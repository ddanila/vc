/*
 * Exact selection contract for the source-built VC 4.05.
 *
 * Selection is an attribute-only panel state, so character substring checks
 * cannot prove it. Pin all four ordinary/selected/cursor/selected-cursor
 * attributes, localized Insert repaint, independent-panel retention, the
 * Gray-* dialog and inversion, selection totals, and Ctrl-R clearing.
 */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (SCREEN_COLS * 50)
#define ATTR_PANEL 0x1b
#define ATTR_SELECTED 0x1e
#define ATTR_CURSOR 0x30
#define ATTR_SELECTED_CURSOR 0x3e
#define ATTR_DIALOG 0x70

struct screen_snapshot {
  unsigned short cells[SCREEN_CELLS];
  int count;
};

static void capture(struct screen_snapshot *screen) {
  screen->count = kviktest_read_screen(screen->cells, SCREEN_CELLS);
}

static unsigned char cell_char(const struct screen_snapshot *screen,
                               int row, int col) {
  int index = row * SCREEN_COLS + col;
  if (index < 0 || index >= screen->count) return 0;
  return (unsigned char)(screen->cells[index] & 0xff);
}

static unsigned char cell_attr(const struct screen_snapshot *screen,
                               int row, int col) {
  int index = row * SCREEN_COLS + col;
  if (index < 0 || index >= screen->count) return 0;
  return (unsigned char)(screen->cells[index] >> 8);
}

static int row_text_is(const struct screen_snapshot *screen, int row, int col,
                       const char *text) {
  while (*text)
    if (cell_char(screen, row, col++) != (unsigned char)*text++) return 0;
  return 1;
}

static int row_attrs_are(const struct screen_snapshot *screen, int row,
                         int first_col, int last_col, unsigned char attr) {
  int col;
  for (col = first_col; col <= last_col; ++col)
    if (cell_attr(screen, row, col) != attr) return 0;
  return 1;
}

static int region_cells_equal(const struct screen_snapshot *a,
                              const struct screen_snapshot *b,
                              int first_row, int last_row,
                              int first_col, int last_col) {
  int row, col;
  for (row = first_row; row <= last_row; ++row)
    for (col = first_col; col <= last_col; ++col) {
      int index = row * SCREEN_COLS + col;
      if (index >= a->count || index >= b->count ||
          a->cells[index] != b->cells[index])
        return 0;
    }
  return 1;
}

static int region_chars_equal(const struct screen_snapshot *a,
                              const struct screen_snapshot *b,
                              int first_row, int last_row,
                              int first_col, int last_col) {
  int row, col;
  for (row = first_row; row <= last_row; ++row)
    for (col = first_col; col <= last_col; ++col)
      if (cell_char(a, row, col) != cell_char(b, row, col)) return 0;
  return 1;
}

static int right_panel_has_no_selected_attributes(
    const struct screen_snapshot *screen) {
  int slot, cell;
  for (slot = 0; slot < 54; ++slot) {
    int row = 2 + slot % 18;
    int col = 41 + (slot / 18) * 13;
    unsigned char expected = slot == 0 ? ATTR_CURSOR : ATTR_PANEL;
    for (cell = 0; cell < 12; ++cell)
      if (cell_attr(screen, row, col + cell) != expected) return 0;
  }
  return 1;
}

static int invert_dialog_is_exact(const struct screen_snapshot *screen) {
  static const unsigned char *rows[] = {
    (const unsigned char *)
      "\xc9\xcd\xcd\xcd\xcd\xcd Invert \xcd\xcd\xcd\xcd\xcd\xbb",
    (const unsigned char *)"\xba Invert the files \xba",
    (const unsigned char *)"\xba *.*              \xba",
    (const unsigned char *)
      "\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc",
  };
  int row, col;
  for (row = 0; row < 4; ++row)
    for (col = 0; col < 20; ++col) {
      unsigned char expected_attr = ATTR_DIALOG;
      if (row == 2 && col >= 2 && col <= 13)
        expected_attr = ATTR_CURSOR;
      if (cell_char(screen, 6 + row, 30 + col) != rows[row][col] ||
          cell_attr(screen, 6 + row, 30 + col) != expected_attr)
        return 0;
    }
  return 1;
}

static void run_tests(void) {
  struct screen_snapshot baseline, inserted, selected_cursor;
  struct screen_snapshot left_active, right_active, dialog, inverted;
  struct screen_snapshot dialog_again, restored, reread;

  /* Default fixture ordering is descending: ZZZ.BAT (30 bytes) is first. */
  kviktest_send_key(0x1910);  /* Ctrl-P: show inactive left panel. */
  kviktest_send_key(0x4700);  /* Home on active right panel. */
  usleep(500000);
  capture(&baseline);
  check(row_text_is(&baseline, 2, 41, "zzz      bat") &&
        row_attrs_are(&baseline, 2, 41, 52, ATTR_CURSOR) &&
        row_attrs_are(&baseline, 3, 41, 52, ATTR_PANEL),
        "baseline has exact ordinary and cursor states");

  kviktest_send_key(0x5200);  /* Insert: select current and move down. */
  usleep(500000);
  capture(&inserted);
  check(row_attrs_are(&inserted, 2, 41, 52, ATTR_SELECTED) &&
        row_attrs_are(&inserted, 3, 41, 52, ATTR_CURSOR),
        "Insert produces exact selected and cursor attributes");
  check(row_text_is(&inserted, 21, 41,
                    "     30 bytes in 1 selected file      "),
        "Insert produces the exact singular selection total");
  check(region_cells_equal(&baseline, &inserted, 1, 22, 0, 39) &&
        region_cells_equal(&baseline, &inserted, 23, 23, 0, 79) &&
        region_chars_equal(&baseline, &inserted, 0, 20, 40, 79),
        "Insert leaves inactive panel, command, frames, and names exact");

  kviktest_send_key(KEY_UP);
  usleep(300000);
  capture(&selected_cursor);
  check(row_attrs_are(&selected_cursor, 2, 41, 52,
                      ATTR_SELECTED_CURSOR) &&
        row_attrs_are(&selected_cursor, 3, 41, 52, ATTR_PANEL),
        "cursor on a selected file has its distinct exact attribute");
  check(region_chars_equal(&inserted, &selected_cursor, 0, 24, 0, 79),
        "moving onto a selected file changes no screen character");

  kviktest_send_key(KEY_TAB);
  usleep(400000);
  capture(&left_active);
  check(row_attrs_are(&left_active, 2, 1, 12, ATTR_CURSOR) &&
        row_attrs_are(&left_active, 2, 41, 52, ATTR_SELECTED),
        "Tab preserves the inactive panel mark exactly");
  kviktest_send_key(KEY_TAB);
  usleep(400000);
  capture(&right_active);
  check(row_attrs_are(&right_active, 2, 1, 12, ATTR_PANEL) &&
        row_attrs_are(&right_active, 2, 41, 52, ATTR_SELECTED_CURSOR),
        "Tab restores the exact selected-cursor state");
  check(region_chars_equal(&selected_cursor, &right_active, 0, 24, 0, 79),
        "Tab round trip changes no screen character");

  kviktest_send_key(0x372a);  /* Gray *: invert-selection dialog. */
  usleep(500000);
  capture(&dialog);
  check(invert_dialog_is_exact(&dialog),
        "Gray-* has exact VC 4.05 dialog cells and attributes");
  kviktest_send_key(KEY_ENTER);
  usleep(700000);
  capture(&inverted);
  check(row_attrs_are(&inverted, 2, 41, 52, ATTR_CURSOR) &&
        row_attrs_are(&inverted, 3, 41, 52, ATTR_SELECTED),
        "invert distinguishes unselected cursor and selected file exactly");
  check(row_text_is(&inverted, 21, 41,
                    "  69,088 bytes in 14 selected files   "),
        "invert produces the exact plural selected byte total");
  check(region_cells_equal(&right_active, &inverted, 1, 22, 0, 39) &&
        region_cells_equal(&right_active, &inverted, 23, 23, 0, 79) &&
        region_chars_equal(&right_active, &inverted, 0, 20, 40, 79),
        "invert preserves inactive panel, command, frame, and names");

  kviktest_send_key(0x372a);
  usleep(500000);
  capture(&dialog_again);
  check(invert_dialog_is_exact(&dialog_again),
        "second Gray-* reproduces the exact dialog");
  kviktest_send_key(KEY_ENTER);
  usleep(700000);
  capture(&restored);
  check(region_cells_equal(&right_active, &restored, 1, 24, 0, 79),
        "two inversions restore every non-clock screen cell exactly");

  kviktest_send_key(0x1312);  /* Ctrl-R explicitly clears bit 6 first. */
  usleep(700000);
  capture(&reread);
  check(row_text_is(&reread, 2, 41, "vc       ini") &&
        row_text_is(&reread, 21, 41, "vc.ini") &&
        right_panel_has_no_selected_attributes(&reread) &&
        !row_text_is(&reread, 21, 41, "     30 bytes in 1 selected file"),
        "Ctrl-R clears selection exactly as F17_04 specifies");
  check(region_cells_equal(&baseline, &reread, 1, 22, 0, 39) &&
        region_cells_equal(&baseline, &reread, 23, 23, 0, 79),
        "Ctrl-R leaves inactive panel and command row exact");
}

TEST_MAIN("test_selection_contract", "coverage_selection_contract.bin")
