/*
 * test_viewer_contract.c: exact VC 4.05 internal-viewer screen contract.
 *
 * The portable viewer tests intentionally accommodate 4.99.09. This oracle
 * is source-build-only and checks characters, VGA attributes, navigation,
 * mode geometry, and exact return to the panels.
 */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (SCREEN_COLS * 50)
#define ATTR_VIEW_STATUS 0x30
#define ATTR_VIEW_BODY 0x1b

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

static int cells_equal(const struct screen_snapshot *a,
                       const struct screen_snapshot *b,
                       int first_row, int last_row) {
  int row, col;
  for (row = first_row; row <= last_row; ++row)
    for (col = 0; col < SCREEN_COLS; ++col) {
      int index = row * SCREEN_COLS + col;
      if (index >= a->count || index >= b->count ||
          a->cells[index] != b->cells[index])
        return 0;
    }
  return 1;
}

static int row_attrs_are(const struct screen_snapshot *screen, int row,
                         int first_col, int last_col, unsigned char attr) {
  int col;
  for (col = first_col; col <= last_col; ++col)
    if (cell_attr(screen, row, col) != attr) return 0;
  return 1;
}

static int wait_for_exact_text_view(struct screen_snapshot *screen) {
  int attempt;

  for (attempt = 0; attempt < 100; ++attempt) {
    capture(screen);
    if (screen->count == 25 * SCREEN_COLS &&
        row_text_is(screen, 0, 0, "View:") &&
        row_text_is(screen, 0, 6, "hello.txt") &&
        row_text_is(screen, 0, 40, "Col 0") &&
        row_text_is(screen, 0, 62, "12 Bytes") &&
        row_text_is(screen, 0, 75, "100%") &&
        row_text_is(screen, 1, 0, "Hello World") &&
        row_attrs_are(screen, 0, 0, 79, ATTR_VIEW_STATUS) &&
        row_attrs_are(screen, 1, 0, 79, ATTR_VIEW_BODY))
      return 1;
    usleep(100000);
  }
  return 0;
}

static void run_tests(void) {
  struct screen_snapshot panels, text, hex, restored;

  check(navigate_to("hello", NULL), "cursor on HELLO.TXT");
  capture(&panels);

  kviktest_send_key(KEY_F3);
  check(wait_for_exact_text_view(&text),
        "F3 enters the exact completed text view");
  check(row_text_is(&text, 0, 0, "View:") &&
        row_text_is(&text, 0, 6, "hello.txt"),
        "text header has exact title and filename columns");
  check(row_text_is(&text, 0, 40, "Col 0") &&
        row_text_is(&text, 0, 62, "12 Bytes") &&
        row_text_is(&text, 0, 75, "100%"),
        "text header has exact column, size, and percentage fields");
  check(row_text_is(&text, 1, 0, "Hello World"),
        "text body starts at row 1 column 0");
  check(row_attrs_are(&text, 0, 0, 79, ATTR_VIEW_STATUS) &&
        row_attrs_are(&text, 1, 0, 79, ATTR_VIEW_BODY),
        "viewer status and body attributes are exact");

  kviktest_send_key(KEY_F4);
  usleep(500000);
  capture(&hex);
  check(row_text_is(&hex, 1, 0, " 00000000  48 65 6C 6C  6F 20 57 6F  72 6C 64 0A"),
        "hex address and grouped byte columns are exact");
  check(row_text_is(&hex, 1, 63, "Hello World"),
        "hex ASCII panel begins at column 63");
  check(row_attrs_are(&hex, 1, 0, 79, ATTR_VIEW_BODY),
        "hex body uses the exact viewer attribute");

  kviktest_send_key(KEY_ESC);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000),
        "Esc returns to panels");
  capture(&restored);
  check(cells_equal(&panels, &restored, 0, 23),
        "viewer exit restores every panel and command cell exactly");
}

TEST_MAIN("test_viewer_contract", "coverage_viewer_contract.bin")
