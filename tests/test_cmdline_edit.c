/*
 * test_cmdline_edit.c: command line editing, Ctrl+B keybar toggle,
 * Ctrl+I insert filenames, Ctrl+[/] insert paths.
 *
 * Regression tests for command line cursor editing features.
 */
#include "test_common.h"

/* Key codes matching BIOS scan codes (scancode << 8 | ASCII). */
#define CTRL_B  0x3002
#define CTRL_D  0x2004
#define CTRL_G  0x2207
#define CTRL_I  0x1709
#define CTRL_K  0x250B
#define CTRL_S  0x1F13
#define CTRL_T  0x1414
#define CTRL_W  0x1117
#define CTRL_Y  0x1519
#define CTRL_LBRACE 0x1A1B  /* Ctrl+[ */
#define CTRL_RBRACE 0x1B1D  /* Ctrl+] */

/* ---- Command line cursor editing ---- */
static void test_cmdline_cursor(void) {
  char buf[81];
  printf("\n--- Command line cursor editing ---\n");

  /* Type "abcdef" on the command line. */
  type_string("abcdef");
  usleep(300000);

  /* Read command line row and verify text appears. */
  kviktest_read_text(23, 0, buf, 81);
  check(strstr(buf, "abcdef") != NULL, "typed text visible on cmdline");

  /* Ctrl+Y — delete entire command line. */
  kviktest_send_key(CTRL_Y);
  usleep(300000);
  kviktest_read_text(23, 0, buf, 81);
  check(strstr(buf, "abcdef") == NULL, "Ctrl+Y cleared command line");

  /* Type "hello", then Ctrl+K to delete to end. */
  type_string("hello");
  usleep(200000);

  /* Move cursor left twice with Ctrl+S. */
  kviktest_send_key(CTRL_S);
  usleep(100000);
  kviktest_send_key(CTRL_S);
  usleep(100000);

  /* Ctrl+K deletes from cursor to end ("lo" deleted, "hel" remains). */
  kviktest_send_key(CTRL_K);
  usleep(300000);
  kviktest_read_text(23, 0, buf, 81);
  check(strstr(buf, "hel") != NULL, "Ctrl+K: 'hel' remains");
  /* "hello" should be gone — just "hel" left. */
  check(strstr(buf, "hello") == NULL, "Ctrl+K: 'hello' gone");

  /* Clean up command line. */
  kviktest_send_key(CTRL_Y);
  usleep(200000);

  check(kviktest_is_running(), "alive after cmdline editing");
}

/* ---- Ctrl+G (delete char) and Ctrl+T (delete word right) ---- */
static void test_cmdline_delete(void) {
  char buf[81];
  printf("\n--- Command line delete operations ---\n");

  /* Type "abc def". */
  type_string("abc def");
  usleep(300000);

  /* Ctrl+W — delete word left. Should delete "def" leaving "abc ". */
  kviktest_send_key(CTRL_W);
  usleep(300000);
  kviktest_read_text(23, 0, buf, 81);
  check(strstr(buf, "def") == NULL, "Ctrl+W deleted word left");

  /* Clean up. */
  kviktest_send_key(CTRL_Y);
  usleep(200000);

  /* Type "foo bar baz", move to start, Ctrl+T deletes word right. */
  type_string("foo bar baz");
  usleep(200000);
  /* Move to beginning: press Ctrl+S enough times. */
  { int i; for (i = 0; i < 15; i++) { kviktest_send_key(CTRL_S); usleep(30000); } }
  usleep(200000);
  /* Ctrl+T — delete word right from beginning. */
  kviktest_send_key(CTRL_T);
  usleep(300000);
  kviktest_read_text(23, 0, buf, 81);
  check(strstr(buf, "foo") == NULL, "Ctrl+T deleted first word");

  /* Clean up. */
  kviktest_send_key(CTRL_Y);
  usleep(200000);
  check(kviktest_is_running(), "alive after delete ops");
}

/* ---- Ctrl+B: toggle function key bar ---- */
static void test_ctrl_b_keybar(void) {
  char buf[81];
  printf("\n--- Ctrl+B keybar toggle ---\n");

  /* Verify keybar is visible initially (row 24 has "Help"). */
  kviktest_read_text(24, 0, buf, 81);
  check(strstr(buf, "Help") != NULL, "keybar visible initially");

  /* Ctrl+B — hide keybar. */
  kviktest_send_key(CTRL_B);
  usleep(500000);
  kviktest_read_text(24, 0, buf, 81);
  check(strstr(buf, "Help") == NULL, "keybar hidden after Ctrl+B");

  /* Ctrl+B — show keybar again. */
  kviktest_send_key(CTRL_B);
  usleep(500000);
  kviktest_read_text(24, 0, buf, 81);
  check(strstr(buf, "Help") != NULL, "keybar restored after second Ctrl+B");

  check(kviktest_is_running(), "alive after keybar toggle");
}

/* ---- Held Alt: function key bar must match original modifier labels ---- */
static void test_alt_held_keybar(void) {
  char buf[81];
  printf("\n--- Held Alt keybar ---\n");

  if (test_is_vc_499()) {
    /* 4.99.09 renders a static F-key bar; modifier-aware updates only
     * fire on real-DOS keyboard interrupts (INT 9h hook), which the
     * test harness can't simulate. Skip the modifier-label asserts. */
    check(1, "skipped (4.99 has static keybar)");
    return;
  }

  kviktest_set_shift_flags(0x08);  /* Alt down. */
  usleep(500000);
  kviktest_read_text(24, 0, buf, 81);
  printf("  keybar: %s\n", buf);
  check(strstr(buf, "1Left") != NULL, "Alt keybar shows Left");
  check(strstr(buf, "2Right") != NULL, "Alt keybar shows Right");
  check(strstr(buf, "3View..") != NULL, "Alt keybar shows View");
  check(strstr(buf, "4Edit..") != NULL, "Alt keybar shows Edit");
  check(strstr(buf, "5Memory") != NULL, "Alt keybar shows Memory");
  check(strstr(buf, "6DirSiz") != NULL, "Alt keybar shows DirSiz");
  check(strstr(buf, "7Find") != NULL, "Alt keybar shows Find");
  check(strstr(buf, "8Histry") != NULL, "Alt keybar shows Histry");
  check(strstr(buf, "9EGA Ln") != NULL, "Alt keybar shows EGA Ln");
  check(strstr(buf, "10Tree") != NULL, "Alt keybar shows Tree");

  kviktest_set_shift_flags(0x00);  /* Alt up. */
  kviktest_send_key(KEY_ESC);      /* Close Alt-alone menu if the reference opens it. */
  usleep(300000);
}

/* ---- Ctrl+I: insert selected filenames ---- */
static void test_ctrl_i_insert(void) {
  char buf[81];
  printf("\n--- Ctrl+I insert selected filenames ---\n");

  /* Ensure clean command line. */
  kviktest_send_key(CTRL_Y);
  usleep(200000);

  /* Navigate to End to land on a file (dirs sort first, files last).
   * Insert skips directories, so Home+Down would land on SUBDIR2. */
  kviktest_send_key(0x4F00);  /* End — last entry is always a file */
  usleep(300000);
  kviktest_send_key(0x4800);  /* Up — second-to-last is also a file */
  usleep(200000);
  kviktest_send_key(0x5200);  /* Insert — select file */
  usleep(500000);

  /* Ctrl+I — insert selected filename into command line. */
  kviktest_send_key(CTRL_I);
  usleep(700000);

  /* Command line should now contain something (the filename). */
  kviktest_read_text(23, 0, buf, 81);
  { int plen = 0;
    /* Skip past the prompt (e.g. "C:\>") to find inserted text. */
    while (buf[plen] && buf[plen] != '>') plen++;
    if (buf[plen] == '>') plen++;
    check((buf[plen] != '\0' && buf[plen] != ' ') || buf[plen+1] != ' ',
          "Ctrl+I inserted filename into cmdline");
  }

  /* Clean up: deselect and clear cmdline. */
  kviktest_send_key(0x372A);  /* numpad * to toggle selection */
  usleep(300000);
  kviktest_send_key(CTRL_Y);
  usleep(200000);
  check(kviktest_is_running(), "alive after Ctrl+I");
}

static void run_tests(void) {
  test_cmdline_cursor();
  test_cmdline_delete();
  test_ctrl_b_keybar();
  test_ctrl_i_insert();
  test_alt_held_keybar();
}

TEST_MAIN("test_cmdline_edit", "coverage_cmdline_edit.bin")
