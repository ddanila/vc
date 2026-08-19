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

  /* The isolated fixture has three 10-byte files, so totals do not depend on
     the configured name/extension/time sort order. */
  kviktest_send_key(0x1910);  /* Ctrl-P: show inactive left panel. */
  kviktest_send_key(0x4700);  /* Home on active right panel. */
  usleep(500000);
  capture(&baseline);
  check(cell_char(&baseline, 2, 41) != ' ' &&
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
                    "     10 bytes in 1 selected file      "),
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
                    "     20 bytes in 2 selected files     "),
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
  check(right_panel_has_no_selected_attributes(&reread) &&
        !row_text_is(&reread, 21, 41, "     10 bytes in 1 selected file"),
        "Ctrl-R clears selection exactly as F17_04 specifies");
  check(region_cells_equal(&baseline, &reread, 1, 22, 0, 39) &&
        region_cells_equal(&baseline, &reread, 23, 23, 0, 79),
        "Ctrl-R leaves inactive panel and command row exact");
}

static int create_fixture_file(const char *directory, const char *name) {
  char path[1024];
  FILE *file;
  int index;
  snprintf(path, sizeof(path), "%s/%s", directory, name);
  file = fopen(path, "wb");
  if (!file) return -1;
  for (index = 0; index < 10; ++index) fputc('A' + index, file);
  return fclose(file);
}

int main(void) {
  char fixture_template[] = "/tmp/vc_selection_XXXXXX";
  char *fixture_dir;
  if (!find_vc_com()) {
    fprintf(stderr, "SKIP: VC.COM not found\n"); return 0;
  }
  fixture_dir = mkdtemp(fixture_template);
  if (!fixture_dir ||
      create_fixture_file(fixture_dir, "ALPHA.DAT") != 0 ||
      create_fixture_file(fixture_dir, "BETA.DAT") != 0 ||
      create_fixture_file(fixture_dir, "GAMMA.DAT") != 0) {
    fprintf(stderr, "FAIL: could not create selection fixtures\n");
    return 1;
  }
  strncpy(g_mount_dir, fixture_dir, sizeof(g_mount_dir) - 1);
  printf("=== test_selection_contract ===\n");
  printf("VC.COM: %s\n", vc_path);
  printf("mount:  %s\n", fixture_dir);
  signal(SIGALRM, watchdog_handler);
  alarm(180);
  kviktest_coverage_enable();
  if (kviktest_start(vc_path, fixture_dir) != 0) {
    fprintf(stderr, "FAIL: could not start kvikdos\n");
    return 1;
  }
  if (!kviktest_wait_for_text(23, 0, "C:\\>", 15000)) {
    fprintf(stderr, "FAIL: VC.COM did not render prompt\n");
    kviktest_stop(); return 1;
  }
  run_tests();
  alarm(0);
  kviktest_coverage_report(vc_path, 55296);
  kviktest_coverage_dump("coverage_selection_contract.bin");
  printf("\nStopping emulator...\n"); fflush(stdout);
  kviktest_stop();
  {
    char *owned = strdup(fixture_dir);
    cleanup_fixtures(owned);
  }
  printf("\n=== test_selection_contract: %d passed, %d failed ===\n",
         g_pass, g_fail);
  fflush(stdout);
  _exit(g_fail > 0 ? 1 : 0);
}
