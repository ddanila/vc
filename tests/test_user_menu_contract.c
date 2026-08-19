/* Exact VC 4.05 F2 User Menu character/attribute oracle. */
#include "test_common.h"

#include <stdint.h>

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)
#define USER_FIRST_FINGERPRINT UINT64_C(0x36632d3f70053a8c)
#define USER_SECOND_FINGERPRINT UINT64_C(0x9f58d4ebd79c57ac)

struct screen_snapshot {
  unsigned short cells[SCREEN_CELLS];
  int count;
};

static void capture(struct screen_snapshot *screen) {
  screen->count = kviktest_read_screen(screen->cells, SCREEN_CELLS);
}

/* The selected fixture timestamp remains visible behind the modal window and
 * DOS converts it through the host timezone. Normalize only that 8.3 info-line
 * date/time field; attributes and every foreground byte remain exact. */
static uint64_t screen_fingerprint(const struct screen_snapshot *screen) {
  uint64_t hash = UINT64_C(1469598103934665603);
  int i;

  if (screen->count != SCREEN_CELLS) return 0;
  for (i = 0; i < screen->count; ++i) {
    unsigned char character = (unsigned char)(screen->cells[i] & 0xff);
    if (i >= 21 * SCREEN_COLS + 65 && i <= 21 * SCREEN_COLS + 78)
      character = 0;
    hash ^= character;
    hash *= UINT64_C(1099511628211);
    hash ^= (unsigned char)(screen->cells[i] >> 8);
    hash *= UINT64_C(1099511628211);
  }
  return hash;
}

static int screens_equal(const struct screen_snapshot *a,
                         const struct screen_snapshot *b) {
  return a->count == b->count &&
         memcmp(a->cells, b->cells,
                (size_t)a->count * sizeof(a->cells[0])) == 0;
}

static int wait_for_fingerprint(struct screen_snapshot *screen,
                                uint64_t expected) {
  int elapsed;
  uint64_t actual = 0;
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(screen);
    actual = screen_fingerprint(screen);
    if (actual == expected) return 1;
    usleep(10000);
  }
  printf("  User Menu fingerprint: 0x%016llx\n",
         (unsigned long long)actual);
  return 0;
}

static int wait_for_restore(const struct screen_snapshot *baseline,
                            struct screen_snapshot *screen) {
  int elapsed;
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(screen);
    if (screens_equal(baseline, screen)) return 1;
    usleep(10000);
  }
  return 0;
}

static int write_user_menu(void) {
  char path[1024];
  FILE *fp;
  snprintf(path, sizeof(path), "%s/VC.MNU", g_mount_dir);
  fp = fopen(path, "wb");
  if (!fp) return 0;
  fputs("A: Assemble current\r\n\tMAKE !\r\n"
        "V: View output\r\n\tLIST !.\r\n", fp);
  return fclose(fp) == 0;
}

static void test_exact_user_menu(void) {
  struct screen_snapshot baseline, first, second, restored;

  check(write_user_menu(), "created deterministic VC.MNU fixture");
  usleep(300000);
  capture(&baseline);

  kviktest_send_key(KEY_F2);
  check(wait_for_fingerprint(&first, USER_FIRST_FINGERPRINT),
        "4.05 User Menu initial characters and attributes are exact");

  kviktest_send_key(KEY_DOWN);
  check(wait_for_fingerprint(&second, USER_SECOND_FINGERPRINT),
        "4.05 User Menu Down focus characters and attributes are exact");

  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&baseline, &restored),
        "4.05 User Menu cancellation restores every screen cell");
}

static void run_tests(void) {
  test_exact_user_menu();
}

TEST_MAIN("test_user_menu_contract", "coverage_user_menu_contract.bin")
