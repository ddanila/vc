/* Exact VC 4.05 Help character/attribute oracle. */
#include "test_common.h"

#include <stdint.h>

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)
#define HELP_INDEX_FINGERPRINT UINT64_C(0x926ab9d4266c9b20)
#define HELP_PAGE_FINGERPRINT  UINT64_C(0xfa96d11770707474)

struct screen_snapshot {
  unsigned short cells[SCREEN_CELLS];
  int count;
};

static void capture(struct screen_snapshot *screen) {
  screen->count = kviktest_read_screen(screen->cells, SCREEN_CELLS);
}

static unsigned char cell_char(const struct screen_snapshot *screen,
                               int row, int col) {
  return (unsigned char)(screen->cells[row * SCREEN_COLS + col] & 0xff);
}

static unsigned char cell_attr(const struct screen_snapshot *screen,
                               int row, int col) {
  return (unsigned char)(screen->cells[row * SCREEN_COLS + col] >> 8);
}

/* Fingerprint both bytes of all 2,000 VGA cells. Unlike a text substring,
 * this pins the complete VC 4.05 character and attribute oracle. */
static uint64_t screen_fingerprint(const struct screen_snapshot *screen) {
  uint64_t hash = UINT64_C(1469598103934665603);
  int i;

  if (screen->count != SCREEN_CELLS) return 0;
  for (i = 0; i < screen->count; ++i) {
    unsigned char character = (unsigned char)(screen->cells[i] & 0xff);
    if (i >= 21 * SCREEN_COLS + 76 && i <= 21 * SCREEN_COLS + 78)
      character = 0;  /* live VC clock digits and a/p suffix */
    hash ^= character;
    hash *= UINT64_C(1099511628211);
    hash ^= (unsigned char)(screen->cells[i] >> 8);
    hash *= UINT64_C(1099511628211);
  }
  return hash;
}

static int help_frame_is_exact(const struct screen_snapshot *screen) {
  int row, col;

  if (screen->count != SCREEN_CELLS ||
      cell_char(screen, 3, 7) != 0xc9 ||
      cell_char(screen, 3, 72) != 0xbb ||
      cell_char(screen, 21, 7) != 0xc8 ||
      cell_char(screen, 21, 72) != 0xbc)
    return 0;
  for (row = 4; row <= 20; ++row)
    if (row != 5 && row != 19 &&
        (cell_char(screen, row, 7) != 0xba ||
         cell_char(screen, row, 72) != 0xba))
      return 0;
  for (col = 8; col < 72; ++col) {
    if ((col < 37 || col > 42) && cell_char(screen, 3, col) != 0xcd)
      return 0;
    if (cell_char(screen, 21, col) != 0xcd)
      return 0;
  }
  return memcmp(&screen->cells[3 * SCREEN_COLS + 37],
                (const unsigned short[]){
                  0x3020, 0x3048, 0x3065, 0x306c, 0x3070, 0x3020
                }, 6 * sizeof(unsigned short)) == 0;
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
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(screen);
    if (screen_fingerprint(screen) == expected) return 1;
    usleep(10000);
  }
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

static void test_exact_help(void) {
  struct screen_snapshot baseline, index, page, restored;

  usleep(300000);
  capture(&baseline);

  kviktest_send_key(KEY_F1);
  check(wait_for_fingerprint(&index, HELP_INDEX_FINGERPRINT) &&
        help_frame_is_exact(&index) &&
        cell_attr(&index, 6, 9) == 0x0f &&
        cell_attr(&index, 20, 34) == 0x0f,
        "4.05 Help index characters and attributes are exact");

  kviktest_send_key(KEY_ENTER);
  check(wait_for_fingerprint(&page, HELP_PAGE_FINGERPRINT) &&
        help_frame_is_exact(&page) &&
        cell_attr(&page, 4, 9) == 0x3f &&
        cell_attr(&page, 20, 16) == 0x0f,
        "4.05 Enter opens the exact selected Help topic");

  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&baseline, &restored),
        "4.05 Escape restores every character and attribute cell");
}

static void run_tests(void) {
  test_exact_help();
}

TEST_MAIN("test_help_contract", "coverage_help_contract.bin")
