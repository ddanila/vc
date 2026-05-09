/*
 * test_common.h: shared boilerplate for split VC.COM e2e test binaries.
 *
 * Each test binary includes this header, defines its test functions,
 * and uses the TEST_MAIN() macro to generate main().
 *
 * Fixtures are copied to a temporary directory per test binary,
 * ensuring full isolation between test groups.
 */

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "../vendor/kvikdos/test_harness.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static int g_pass, g_fail;
static char g_mount_dir[512];

/* dump_screen must be defined before check() so it can be called on failure. */
static void dump_screen(void) {
  int r;
  char buf[81];
  printf("--- screen dump ---\n");
  for (r = 0; r < kviktest_get_rows(); ++r) {
    kviktest_read_text(r, 0, buf, 81);
    printf("%02d|%s|\n", r, buf);
  }
  printf("-------------------\n");
}

static void check(int ok, const char *label) {
  if (ok) {
    printf("  PASS: %s\n", label);
    ++g_pass;
  } else {
    printf("  FAIL: %s\n", label);
    dump_screen();
    ++g_fail;
  }
}

/* IBM PC AT scan codes indexed by ASCII value.
 * Lowercase and uppercase letters share the same scan code. */
static const unsigned char ascii_to_scancode[128] = {
  /* 0x00-0x0F */ 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  /* 0x10-0x1F */ 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  /*   ! " # $ % & ' ( ) * + , - . / */
  0x39,0x02,0x28,0x04,0x05,0x06,0x08,0x28,
  0x0A,0x0B,0x09,0x0D,0x33,0x0C,0x34,0x35,
  /* 0 1 2 3 4 5 6 7 8 9 : ; < = > ? */
  0x0B,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
  0x09,0x0A,0x27,0x27,0x33,0x0D,0x34,0x35,
  /* @ A B C D E F G H I J K L M N O */
  0x03,0x1E,0x30,0x2E,0x20,0x12,0x21,0x22,
  0x23,0x17,0x24,0x25,0x26,0x32,0x31,0x18,
  /* P Q R S T U V W X Y Z [ \ ] ^ _ */
  0x19,0x10,0x13,0x1F,0x14,0x16,0x2F,0x11,
  0x2D,0x15,0x2C,0x1A,0x2B,0x1B,0x07,0x0C,
  /* ` a b c d e f g h i j k l m n o */
  0x29,0x1E,0x30,0x2E,0x20,0x12,0x21,0x22,
  0x23,0x17,0x24,0x25,0x26,0x32,0x31,0x18,
  /* p q r s t u v w x y z { | } ~ DEL */
  0x19,0x10,0x13,0x1F,0x14,0x16,0x2F,0x11,
  0x2D,0x15,0x2C,0x1A,0x2B,0x1B,0x29,0x00,
};

/* Type a string as DOS keystrokes (scan code + ASCII). */
static void type_string(const char *s) {
  while (*s) {
    unsigned char c = (unsigned char)*s;
    unsigned char sc = (c < 128) ? ascii_to_scancode[c] : 0;
    if (sc) {
      kviktest_send_key((sc << 8) | c);
      usleep(30000);
    }
    ++s;
  }
}

/* Navigate to a file by name in the active panel.
   Checks both left (col 0) and right (col 41) info lines at row 21,
   so it works regardless of which panel is active. */
static int navigate_to(const char *name, const char *name_alt) {
  int tries;
  char buf[41];
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  for (tries = 0; tries < 25; ++tries) {
    kviktest_read_text(21, 41, buf, 39);
    if (strstr(buf, name) || (name_alt && strstr(buf, name_alt)))
      return 1;
    kviktest_read_text(21, 0, buf, 39);
    if (strstr(buf, name) || (name_alt && strstr(buf, name_alt)))
      return 1;
    kviktest_send_key(KEY_DOWN);
    usleep(300000);
  }
  return 0;
}

/* ---- Host filesystem verification helpers ---- */

/* Check if a file/dir exists in the mounted fixture directory. */
static int host_path_exists(const char *name) {
  char path[1024];
  snprintf(path, sizeof(path), "%s/%s", g_mount_dir, name);
  return access(path, F_OK) == 0;
}

/* Check if a path is a directory in the mounted fixture directory. */
static int host_is_dir(const char *name) {
  char path[1024];
  struct stat st;
  snprintf(path, sizeof(path), "%s/%s", g_mount_dir, name);
  return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

/* Check if two files in the fixture directory have identical content. */
static int host_files_match(const char *name_a, const char *name_b) {
  char pa[1024], pb[1024];
  FILE *fa, *fb;
  int match = 1;
  int ca, cb;
  snprintf(pa, sizeof(pa), "%s/%s", g_mount_dir, name_a);
  snprintf(pb, sizeof(pb), "%s/%s", g_mount_dir, name_b);
  fa = fopen(pa, "rb");
  fb = fopen(pb, "rb");
  if (!fa || !fb) { if (fa) fclose(fa); if (fb) fclose(fb); return 0; }
  while ((ca = fgetc(fa)) != EOF) {
    cb = fgetc(fb);
    if (ca != cb) { match = 0; break; }
  }
  if (match && fgetc(fb) != EOF) match = 0;
  fclose(fa); fclose(fb);
  return match;
}

static char vc_path[1024];

static int find_vc_com(void) {
  const char *env = getenv("VC_BINARY");
  const char *candidates[] = {
    "ORIG_VC/BIN/VC.COM", "../ORIG_VC/BIN/VC.COM", NULL
  };
  int i;
  if (env && env[0]) {
    if (access(env, R_OK) == 0) {
      strncpy(vc_path, env, sizeof(vc_path) - 1);
      return 1;
    }
    fprintf(stderr, "VC_BINARY=%s: not found\n", env);
    return 0;
  }
  for (i = 0; candidates[i]; ++i) {
    if (access(candidates[i], R_OK) == 0) {
      strncpy(vc_path, candidates[i], sizeof(vc_path) - 1);
      return 1;
    }
  }
  return 0;
}

/* Copy master fixtures to a temporary directory.
 * Returns 0 on success. Sets *out_dir to allocated path (caller frees).
 * If VC_OVL_PATH is set in the environment, that file is copied into the
 * temp dir as VC.OVL — VC 4.99.09 expects its overlay to live next to
 * VC.COM. Harmless for 4.05 since the env var is unset there. */
static int setup_fixtures(const char *master_dir, char **out_dir) {
  char tmpl[] = "/tmp/vc_test_XXXXXX";
  char *tmp = mkdtemp(tmpl);
  char cmd[512];
  const char *ovl = getenv("VC_OVL_PATH");
  int ret;
  if (!tmp) { perror("mkdtemp"); return -1; }
  if (access(master_dir, R_OK) != 0) {
    fprintf(stderr, "setup_fixtures: master dir not accessible: %s\n", master_dir);
    rmdir(tmp);
    return -1;
  }
  /* Copy fixtures, then strip the per-version INIs and the generic
   * VC.INI; the harness substitutes the right one below. The default
   * for plain VC is still "no INI, clean state" so the existing 4.05
   * tests run as before. */
  snprintf(cmd, sizeof(cmd),
           "cp -a '%s'/* '%s'/ 2>/dev/null; rm -f '%s'/VC.INI '%s'/vc.ini '%s'/VC.4.*.INI",
           master_dir, tmp, tmp, tmp, tmp);
  ret = system(cmd);
  if (ret < 0 || !WIFEXITED(ret) || WEXITSTATUS(ret) != 0) {
    fprintf(stderr, "setup_fixtures: command failed (exit %d): %s\n",
            WIFEXITED(ret) ? WEXITSTATUS(ret) : -1, cmd);
    snprintf(cmd, sizeof(cmd), "rm -rf '%s'", tmp);
    system(cmd);
    return -1;
  }
  if (ovl && ovl[0]) {
    /* kvikdos presents the program as C:\KVIKPROG.COM (kvikdos.c:2757),
     * so 4.99.09 looks for KVIKPROG.OVL. Copy the overlay under that
     * name AND under VC.OVL — VC.OVL handles any future host that
     * preserves the real basename. Also drop in the captured 4.99 INI
     * (`tests/fixtures/VC.4.99.09.INI`, generated by capture_vcini.sh)
     * as VC.INI so VC reaches its default panel instead of getting
     * stuck on the "Can not read setup file" / "Can't read disk"
     * Error dialog under kvikdos. */
    snprintf(cmd, sizeof(cmd),
             "cp '%s' '%s'/KVIKPROG.OVL && cp '%s' '%s'/VC.OVL",
             ovl, tmp, ovl, tmp);
    ret = system(cmd);
    if (ret < 0 || !WIFEXITED(ret) || WEXITSTATUS(ret) != 0) {
      fprintf(stderr, "setup_fixtures: failed to copy overlay from %s\n", ovl);
      snprintf(cmd, sizeof(cmd), "rm -rf '%s'", tmp);
      system(cmd);
      return -1;
    }
    /* Best-effort INI install. Missing INI is not fatal — VC will
     * boot to its banner and tests can still inspect the screen. */
    snprintf(cmd, sizeof(cmd),
             "cp '%s/VC.4.99.09.INI' '%s'/VC.INI 2>/dev/null",
             master_dir, tmp);
    (void)!system(cmd);
  }
  *out_dir = strdup(tmp);
  return 0;
}

static void cleanup_fixtures(char *dir) {
  char cmd[512];
  if (!dir) return;
  snprintf(cmd, sizeof(cmd), "rm -rf '%s'", dir);
  system(cmd);
  free(dir);
}

/* Watchdog: kill the test if it exceeds the time limit. */
static void watchdog_handler(int sig) {
  (void)sig;
  fprintf(stderr, "\nTIMEOUT: test exceeded time limit\n");
  dump_screen();
  /* dump_screen uses printf to stdout; without flushing, _exit drops
   * the buffered output and the diagnostic dump is lost. Cost us a
   * 2b.0 debugging round before this was noticed. */
  fflush(stdout);
  fflush(stderr);
  _exit(2);
}

/*
 * TEST_MAIN(name, cov_file) — generate main() for a test binary.
 *
 * Usage:
 *   static void run_tests(void) {
 *     test_foo();
 *     test_bar();
 *   }
 *   TEST_MAIN("test_basic", "coverage_basic.bin")
 *
 * The run_tests() function must be defined before this macro.
 */
#define TEST_MAIN(NAME, COV_FILE) \
int main(int argc, char **argv) { \
  char *fixture_dir = NULL; \
  const char *mount_dir; \
  if (!find_vc_com()) { \
    fprintf(stderr, "SKIP: VC.COM not found\n"); return 0; \
  } \
  if (argc > 1) { \
    mount_dir = argv[1]; \
  } else { \
    if (setup_fixtures("tests/fixtures", &fixture_dir) != 0) { \
      fprintf(stderr, "FAIL: could not create fixture dir\n"); return 1; \
    } \
    mount_dir = fixture_dir; \
  } \
  strncpy(g_mount_dir, mount_dir, sizeof(g_mount_dir) - 1); \
  printf("=== %s ===\n", NAME); \
  printf("VC.COM: %s\n", vc_path); \
  printf("mount:  %s\n", mount_dir); \
  signal(SIGALRM, watchdog_handler); \
  alarm(180); \
  kviktest_coverage_enable(); \
  if (kviktest_start(vc_path, mount_dir) != 0) { \
    fprintf(stderr, "FAIL: could not start kvikdos\n"); \
    cleanup_fixtures(fixture_dir); return 1; \
  } \
  if (!kviktest_wait_for_text(23, 0, "C:\\>", 15000)) { \
    fprintf(stderr, "FAIL: VC.COM did not render prompt\n"); \
    kviktest_stop(); cleanup_fixtures(fixture_dir); return 1; \
  } \
  run_tests(); \
  alarm(0); \
  kviktest_coverage_report(vc_path, 55296); \
  kviktest_coverage_dump(COV_FILE); \
  printf("\nStopping emulator...\n"); fflush(stdout); \
  kviktest_stop(); \
  cleanup_fixtures(fixture_dir); \
  printf("\n=== %s: %d passed, %d failed ===\n", NAME, g_pass, g_fail); \
  fflush(stdout); \
  _exit(g_fail > 0 ? 1 : 0); \
}

#endif /* TEST_COMMON_H */
