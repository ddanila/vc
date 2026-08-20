/* Exact VC 4.05 Find character/attribute and transition oracle. */
#include "test_common.h"

#include <stdint.h>

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)
#define ALT_F7 0x6e00
#define CTRL_Y 0x1519
#define FIND_DIALOG_FINGERPRINT UINT64_C(0xa1114bed06337fce)

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

static int row_region_is(const struct screen_snapshot *screen, int row, int col,
                         int width, const char *text) {
  int offset;
  if ((int)strlen(text) != width) return 0;
  for (offset = 0; offset < width; ++offset)
    if (cell_char(screen, row, col + offset) !=
        (unsigned char)text[offset])
      return 0;
  return 1;
}

static int row_prefix_then_zero(const struct screen_snapshot *screen,
                                int row, int col, int width,
                                const char *prefix) {
  int offset;
  int prefix_length = (int)strlen(prefix);
  if (prefix_length > width) return 0;
  for (offset = 0; offset < width; ++offset) {
    unsigned char expected = offset < prefix_length ?
                             (unsigned char)prefix[offset] :
                             (offset == width - 1 ? ' ' : 0);
    if (cell_char(screen, row, col + offset) != expected) return 0;
  }
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

static int request_fields_are_exact(const struct screen_snapshot *screen,
                                    const char *file_field) {
  int col;
  if (!row_text_is(screen, 17, 8, "File name:") ||
      !row_region_is(screen, 18, 8, 64, file_field) ||
      !row_text_is(screen, 19, 8, "Containing:"))
    return 0;
  for (col = 8; col <= 71; ++col)
    if (cell_attr(screen, 18, col) != 0x0f ||
        cell_attr(screen, 20, col) != 0x0f)
      return 0;
  return 1;
}

static int result_chars_are_exact(const struct screen_snapshot *screen,
                                  int *alpha_first) {
  static const char alpha[] =
    "    alpha.doc                 5   A...    8-18-2026    7:00:32p   ";
  static const char beta[] =
    "    beta.doc                  2   A...    8-18-2026    7:00:32p   ";
  static const char blank[] =
    "                                                                  ";
  static const char buttons[] =
    "              [ New search ]   [ Go to ]   [ Quit ]               ";
  int row, col;

  if (!find_frame_is_exact(screen) ||
      !row_prefix_then_zero(screen, 4, 7, 66, " C:\\") ||
      !row_prefix_then_zero(screen, 19, 7, 66, " 2 files found.") ||
      !row_region_is(screen, 20, 7, 66, buttons))
    return 0;

  /* VC preserves DOS FindFirst order. Kvikdos therefore may expose either
   * host-directory order; pin both complete records, not one filesystem's
   * incidental ordering. */
  if (row_region_is(screen, 5, 7, 66, alpha) &&
      row_region_is(screen, 6, 7, 66, beta))
    *alpha_first = 1;
  else if (row_region_is(screen, 5, 7, 66, beta) &&
           row_region_is(screen, 6, 7, 66, alpha))
    *alpha_first = 0;
  else
    return 0;

  for (row = 7; row <= 17; ++row)
    if (!row_region_is(screen, row, 7, 66, blank)) return 0;

  if (cell_char(screen, 18, 6) != 0xc7 ||
      cell_char(screen, 18, 73) != 0xb6)
    return 0;
  for (col = 7; col <= 72; ++col)
    if (cell_char(screen, 18, col) != 0xc4) return 0;
  return 1;
}

static int result_attrs_are_exact(const struct screen_snapshot *screen,
                                  int focused_row) {
  int row, col;
  for (row = 3; row <= 21; ++row)
    for (col = 6; col <= 73; ++col) {
      unsigned char expected = 0x3f;
      if (row == focused_row && col >= 8 && col <= 71)
        expected = 0x0f;
      if (row == 20 && col >= 38 && col <= 46)
        expected = 0x0f;
      if (cell_attr(screen, row, col) != expected)
        return 0;
    }
  return 1;
}

static int wait_for_results(struct screen_snapshot *screen, int focused_row,
                            int *alpha_first) {
  int elapsed;
  for (elapsed = 0; elapsed < 6000; elapsed += 10) {
    capture(screen);
    if (result_chars_are_exact(screen, alpha_first) &&
        result_attrs_are_exact(screen, focused_row))
      return 1;
    usleep(10000);
  }
  return 0;
}

static int same_chars_except_clock(const struct screen_snapshot *a,
                                   const struct screen_snapshot *b) {
  int row, col;
  if (a->count != SCREEN_CELLS || b->count != SCREEN_CELLS) return 0;
  for (row = 0; row < 25; ++row)
    for (col = 0; col < SCREEN_COLS; ++col) {
      if (row == 21 && col >= 75 && col <= 78) continue;
      if (cell_char(a, row, col) != cell_char(b, row, col)) return 0;
    }
  return 1;
}

static int goto_panel_is_exact(const struct screen_snapshot *screen,
                               int alpha_selected) {
  static const char alpha_panel[] = "alpha    doc";
  static const char beta_panel[] = "beta     doc";
  static const char alpha_info[] =
    "alpha.doc            5  8-18-26  7:00p";
  static const char beta_info[] =
    "beta.doc             2  8-18-26  7:00p";
  const char *panel = alpha_selected ? alpha_panel : beta_panel;
  const char *info = alpha_selected ? alpha_info : beta_info;
  int row, col, selected_row = -1;

  if (screen->count != SCREEN_CELLS ||
      !row_region_is(screen, 21, 41, 38, info))
    return 0;

  for (row = 2; row <= 19; ++row)
    if (row_region_is(screen, row, 41, 12, panel)) {
      selected_row = row;
      break;
    }
  if (selected_row < 0) return 0;
  for (col = 41; col <= 52; ++col)
    if (cell_attr(screen, selected_row, col) != 0x30) return 0;
  for (col = 41; col <= 78; ++col)
    if (cell_attr(screen, 21, col) != 0x1b) return 0;
  return 1;
}

static int wait_for_goto(struct screen_snapshot *screen,
                         int alpha_selected) {
  int elapsed;
  for (elapsed = 0; elapsed < 5000; elapsed += 10) {
    capture(screen);
    if (goto_panel_is_exact(screen, alpha_selected)) return 1;
    usleep(10000);
  }
  return 0;
}

static void test_exact_find(void) {
  struct screen_snapshot baseline, dialog, restored, results, moved, target;
  static const char default_field[] =
    "\\*.*                                                            ";
  static const char doc_field[] =
    "*.doc                                                           ";
  int alpha_first = 0, moved_alpha_first = 0;

  usleep(300000);
  capture(&baseline);

  kviktest_send_key(ALT_F7);
  check(wait_for_fingerprint(&dialog, FIND_DIALOG_FINGERPRINT, 3000) &&
        find_frame_is_exact(&dialog) &&
        request_fields_are_exact(&dialog, default_field),
        "4.05 Find request characters and attributes are exact");

  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&baseline, &restored),
        "4.05 Find request Escape restores every non-clock cell");

  kviktest_send_key(ALT_F7);
  check(wait_for_fingerprint(&dialog, FIND_DIALOG_FINGERPRINT, 3000),
        "4.05 Find request reopens exactly");
  kviktest_send_key(CTRL_Y);
  type_string("*.doc");
  usleep(100000);
  capture(&restored);
  check(find_frame_is_exact(&restored) &&
        request_fields_are_exact(&restored, doc_field),
        "4.05 Find filename edit characters and attributes are exact");
  kviktest_send_key(KEY_ENTER);
  {
    int result_ok = wait_for_results(&results, 6, &alpha_first);
    check(result_ok, "4.05 Find scan reaches the exact two-result screen");
  }

  kviktest_send_key(KEY_UP);
  check(wait_for_results(&moved, 5, &moved_alpha_first) &&
        moved_alpha_first == alpha_first &&
        same_chars_except_clock(&results, &moved),
        "4.05 Find Up changes only the exact cursor attributes");

  kviktest_send_key(KEY_ENTER);
  {
    int goto_ok = wait_for_goto(&target, alpha_first);
    check(goto_ok,
          "4.05 Find Go to selects the focused result in the active panel");
  }
}

static void run_tests(void) {
  test_exact_find();
}

TEST_MAIN("test_find_contract", "coverage_find_contract.bin")
