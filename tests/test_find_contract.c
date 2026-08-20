/* Exact VC 4.05 Find character/attribute and transition oracle. */
#include "test_common.h"

#include <stdint.h>

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)
#define ALT_F7 0x6e00
#define FIND_DIALOG_FINGERPRINT UINT64_C(0xa1114bed06337fce)
#define FIND_RESULTS_FINGERPRINT UINT64_C(0x74bd5c5991890edd)
#define FIND_RESULTS_UP_FINGERPRINT UINT64_C(0x2aab6f5618630afd)
#define FIND_GOTO_FINGERPRINT UINT64_C(0xe40213a91e57f80c)

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

static int row_text_is(const struct screen_snapshot *screen, int row, int col,
                       const char *text) {
  while (*text)
    if (cell_char(screen, row, col++) != (unsigned char)*text++) return 0;
  return 1;
}

/* Pin both bytes of every cell in the complete framed Find region. The
 * shadow and outer canvas are checked structurally or against the baseline. */
static uint64_t screen_fingerprint(const struct screen_snapshot *screen) {
  uint64_t hash = UINT64_C(1469598103934665603);
  int row, col;

  if (screen->count != SCREEN_CELLS) return 0;
  for (row = 3; row <= 20; ++row)
    for (col = 7; col <= 72; ++col) {
      int i = row * SCREEN_COLS + col;
      unsigned char character = (unsigned char)(screen->cells[i] & 0xff);
      hash ^= character;
      hash *= UINT64_C(1099511628211);
      hash ^= (unsigned char)(screen->cells[i] >> 8);
      hash *= UINT64_C(1099511628211);
    }
  return hash;
}

static int screens_equal_except_clock(const struct screen_snapshot *a,
                                      const struct screen_snapshot *b) {
  int row, col;
  if (a->count != SCREEN_CELLS || b->count != SCREEN_CELLS) return 0;
  for (row = 0; row < 25; ++row)
    for (col = 0; col < SCREEN_COLS; ++col) {
      int i = row * SCREEN_COLS + col;
      if (row == 21 && col >= 75 && col <= 78) continue;
      if (a->cells[i] != b->cells[i]) return 0;
    }
  return 1;
}

static int wait_for_fingerprint(struct screen_snapshot *screen,
                                uint64_t expected, int timeout_ms) {
  int elapsed;
  for (elapsed = 0; elapsed < timeout_ms; elapsed += 10) {
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
    if (screens_equal_except_clock(baseline, screen)) return 1;
    usleep(10000);
  }
  return 0;
}

static int find_frame_is_exact(const struct screen_snapshot *screen) {
  int row, col;
  if (screen->count != SCREEN_CELLS ||
      cell_char(screen, 3, 6) != 0xc9 ||
      cell_char(screen, 3, 73) != 0xbb ||
      cell_char(screen, 21, 6) != 0xc8 ||
      cell_char(screen, 21, 73) != 0xbc ||
      !row_text_is(screen, 3, 34, " Find File "))
    return 0;
  for (row = 4; row <= 20; ++row)
    if (row != 16 && row != 18 &&
        (cell_char(screen, row, 6) != 0xba ||
         cell_char(screen, row, 73) != 0xba))
      return 0;
  for (col = 7; col < 73; ++col)
    if (cell_char(screen, 21, col) != 0xcd) return 0;
  return 1;
}

static int request_fields_are_exact(const struct screen_snapshot *screen) {
  int col;
  if (!row_text_is(screen, 17, 8, "File name:") ||
      !row_text_is(screen, 18, 8, "\\*.*") ||
      !row_text_is(screen, 19, 8, "Containing:"))
    return 0;
  for (col = 8; col <= 71; ++col)
    if (cell_attr(screen, 18, col) != 0x0f ||
        cell_attr(screen, 20, col) != 0x0f)
      return 0;
  return 1;
}

static int result_focus_is_exact(const struct screen_snapshot *screen,
                                 int focused_row) {
  int row, col;
  if (!row_text_is(screen, 19, 8, "16 files found.") ||
      !row_text_is(screen, 20, 21, "[ New search ]") ||
      !row_text_is(screen, 20, 38, "[ Go to ]") ||
      !row_text_is(screen, 20, 50, "[ Quit ]") ||
      !row_text_is(screen, 16, 11, "alpha.doc") ||
      !row_text_is(screen, 17, 11, "aaa.com"))
    return 0;
  for (row = 16; row <= 17; ++row)
    for (col = 8; col <= 71; ++col)
      if (cell_attr(screen, row, col) !=
          (unsigned char)(row == focused_row ? 0x0f : 0x3f))
        return 0;
  return 1;
}

static void test_exact_find(void) {
  struct screen_snapshot baseline, dialog, restored, results, moved, target;

  usleep(300000);
  capture(&baseline);

  kviktest_send_key(ALT_F7);
  check(wait_for_fingerprint(&dialog, FIND_DIALOG_FINGERPRINT, 3000) &&
        find_frame_is_exact(&dialog) && request_fields_are_exact(&dialog),
        "4.05 Find request characters and attributes are exact");

  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&baseline, &restored),
        "4.05 Find request Escape restores every non-clock cell");

  kviktest_send_key(ALT_F7);
  check(wait_for_fingerprint(&dialog, FIND_DIALOG_FINGERPRINT, 3000),
        "4.05 Find request reopens exactly");
  kviktest_send_key(KEY_ENTER);
  check(wait_for_fingerprint(&results, FIND_RESULTS_FINGERPRINT, 6000) &&
        find_frame_is_exact(&results) && result_focus_is_exact(&results, 17),
        "4.05 Find scan reaches the exact auto-scrolled result screen");

  kviktest_send_key(KEY_UP);
  check(wait_for_fingerprint(&moved, FIND_RESULTS_UP_FINGERPRINT, 3000) &&
        result_focus_is_exact(&moved, 16),
        "4.05 Find Up changes only the source cursor focus");

  kviktest_send_key(KEY_ENTER);
  check(wait_for_fingerprint(&target, FIND_GOTO_FINGERPRINT, 5000) &&
        row_text_is(&target, 21, 41, "alpha.doc"),
        "4.05 Find Go to selects the focused result in the active panel");
}

static void run_tests(void) {
  test_exact_find();
}

TEST_MAIN("test_find_contract", "coverage_find_contract.bin")
