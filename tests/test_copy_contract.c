/*
 * Exact F5 Copy confirmation contract for the source-built VC 4.05.
 *
 * The broad file-operation suite proves host-side effects. This oracle pins
 * the source UI and the current-versus-selected decision that the CP/M port
 * translates; it deliberately does not apply to the different 4.99 alpha.
 */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (SCREEN_COLS * 50)
#define ATTR_DIALOG 0x70
#define ATTR_EDITOR 0x30

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

static int row_contains(const struct screen_snapshot *screen, int row,
                        const char *text) {
  int col;
  size_t length = strlen(text);
  for (col = 0; col + (int)length <= SCREEN_COLS; ++col)
    if (row_text_is(screen, row, col, text)) return 1;
  return 0;
}

static int dialog_is_exact(const struct screen_snapshot *screen,
                           const char *description) {
  int row, col;
  if (!row_text_is(screen, 6, 37, " Copy ") ||
      !row_text_is(screen, 7, 8, description) ||
      !row_text_is(screen, 8, 8, "C:\\") ||
      !row_text_is(screen, 10, 22,
                   "[ Copy ]   [ F10-Tree ]   [ Cancel ]"))
    return 0;
  for (row = 6; row <= 11; ++row)
    for (col = 6; col <= 73; ++col) {
      unsigned char expected =
        (row == 8 && col >= 8 && col <= 71) ? ATTR_EDITOR : ATTR_DIALOG;
      if (cell_attr(screen, row, col) != expected) return 0;
    }
  if (cell_char(screen, 6, 6) != 0xc9 ||
      cell_char(screen, 6, 73) != 0xbb ||
      cell_char(screen, 9, 6) != 0xc7 ||
      cell_char(screen, 9, 73) != 0xb6 ||
      cell_char(screen, 11, 6) != 0xc8 ||
      cell_char(screen, 11, 73) != 0xbc)
    return 0;
  for (col = 7; col <= 72; ++col) {
    if ((col < 37 || col > 42) && cell_char(screen, 6, col) != 0xcd)
      return 0;
    if (cell_char(screen, 9, col) != 0xc4 ||
        cell_char(screen, 11, col) != 0xcd)
      return 0;
  }
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

static int cells_equal_except_clock(const struct screen_snapshot *a,
                                    const struct screen_snapshot *b) {
  int row, col;
  for (row = 0; row <= 24; ++row)
    for (col = 0; col < SCREEN_COLS; ++col) {
      int index = row * SCREEN_COLS + col;
      if (row == 0 && col >= 67) continue;
      if (index >= a->count || index >= b->count ||
          a->cells[index] != b->cells[index])
        return 0;
    }
  return 1;
}

static void run_tests(void) {
  struct screen_snapshot single_before, single_dialog, single_restored;
  struct screen_snapshot selected_before, selected_dialog, selected_restored;

  /* A visible inactive panel makes its C:\ path the source default. */
  kviktest_send_key(0x1910);  /* Ctrl-P */
  check(navigate_to("hello", NULL), "HELLO.TXT is the current source");
  usleep(300000);
  capture(&single_before);
  kviktest_send_key(KEY_F5);
  usleep(500000);
  capture(&single_dialog);
  check(dialog_is_exact(&single_dialog, "Copy \"hello.txt\" to"),
        "F5 has exact single-file Copy dialog and inactive target default");
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  capture(&single_restored);
  check(cells_equal_except_clock(&single_before, &single_restored),
        "Escape restores every non-clock panel and command cell");

  kviktest_send_key(0x4700);  /* Home */
  kviktest_send_key(0x5200);  /* Insert */
  kviktest_send_key(0x5200);  /* Insert */
  usleep(500000);
  capture(&selected_before);
  kviktest_send_key(KEY_F5);
  usleep(500000);
  capture(&selected_dialog);
  check(dialog_is_exact(&selected_dialog, "Copy 2 files to"),
        "selected set takes precedence and keeps the exact Copy dialog");
  check(row_contains(&selected_dialog, 21, " bytes in 2 selected files") &&
        region_cells_equal(&selected_before, &selected_dialog,
                           21, 21, 40, 79),
        "Copy dialog leaves the fixture-exact selected-file total visible");
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  capture(&selected_restored);
  check(cells_equal_except_clock(&selected_before, &selected_restored),
        "selected Copy cancellation restores exact marks and panels");
}

TEST_MAIN("test_copy_contract", "coverage_copy_contract.bin")
