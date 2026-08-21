/* Exact VC 4.05 F9 menu character/attribute oracle. */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)
#define KEY_END 0x4F00

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

static int menu_bar_is_exact(const struct screen_snapshot *screen,
                             int focus_col, int focus_width) {
  static const char bar[] =
    "    Left    Files    Commands    Options    Right";
  int col;

  if (screen->count != SCREEN_CELLS) return 0;
  for (col = 0; col < SCREEN_COLS; ++col) {
    unsigned char expected = col < (int)sizeof(bar) - 1 ?
                             (unsigned char)bar[col] : ' ';
    unsigned char attr = col >= focus_col &&
                         col < focus_col + focus_width ? 0x0f : 0x30;
    if (cell_char(screen, 0, col) != expected ||
        cell_attr(screen, 0, col) != attr)
      return 0;
  }
  return 1;
}

static int panel_menu_is_exact(const struct screen_snapshot *screen,
                               int base, int focus_col, int hint_digit,
                               int visible) {
  static const unsigned char *const rows[] = {
    (const unsigned char *)"\xfb Brief                ",
    (const unsigned char *)"  Full                 ",
    (const unsigned char *)"  Info                 ",
    (const unsigned char *)"  Tree                 ",
    (const unsigned char *)"  On/Off       Ctrl-F1 ",
    NULL,
    (const unsigned char *)"  Name                 ",
    (const unsigned char *)"  eXtension            ",
    (const unsigned char *)"  tiMe                 ",
    (const unsigned char *)"  Size                 ",
    (const unsigned char *)"\xfb Unsorted             ",
    NULL,
    (const unsigned char *)"  Re-read      Ctrl-R  ",
    (const unsigned char *)"  fiLter...    Ctrl-F  ",
    (const unsigned char *)"  Drive...     Alt-F1  ",
  };
  static const unsigned char hot_col[] = {
    2, 2, 2, 2, 2, 0, 2, 3, 4, 2, 2, 0, 2, 4, 2
  };
  int row, col;

  if (!menu_bar_is_exact(screen, focus_col, focus_col == 2 ? 8 : 9))
    return 0;

  if (cell_char(screen, 1, base) != 0xda ||
      cell_char(screen, 1, base + 24) != 0xbf ||
      cell_char(screen, 17, base) != 0xc0 ||
      cell_char(screen, 17, base + 24) != 0xd9)
    return 0;
  for (col = base + 1; col < base + 24; ++col) {
    if (cell_char(screen, 1, col) != 0xc4 ||
        cell_char(screen, 17, col) != 0xc4)
      return 0;
  }

  for (row = 0; row < 15; ++row) {
    int screen_row = row + 2;
    if (cell_char(screen, screen_row, base) != 0xb3 ||
        cell_char(screen, screen_row, base + 24) != 0xb3 ||
        cell_attr(screen, screen_row, base) != 0x30 ||
        cell_attr(screen, screen_row, base + 24) != 0x30)
      return 0;
    for (col = 0; col < 23; ++col) {
      unsigned char expected_char = rows[row] ? rows[row][col] :
                                              (col == 0 || col == 22 ?
                                               ' ' : 0xc4);
      unsigned char expected_attr = rows[row] ?
          (row == 0 ? 0x0f : 0x3f) : 0x30;
      if (!visible && row == 13)
        expected_attr = col == 1 ? 0x33 : 0x30;
      else if (rows[row] && col == hot_col[row])
        expected_attr = row == 0 ? 0x0e : 0x3e;
      if (!visible && (row == 0 || row == 10) && col == 0)
        expected_char = ' ';
      if (!visible && row == 4 && col == 0)
        expected_char = 0xfb;
      if (!visible && row == 13 && col == 1)
        expected_char = '-';
      if (row == 4 && col == 21) expected_char = (unsigned char)hint_digit;
      if (row == 14 && col == 20) expected_char = (unsigned char)hint_digit;
      if (cell_char(screen, screen_row, base + 1 + col) != expected_char ||
          cell_attr(screen, screen_row, base + 1 + col) != expected_attr)
        return 0;
    }
  }
  return 1;
}

static int screens_equal(const struct screen_snapshot *a,
                         const struct screen_snapshot *b) {
  return a->count == b->count &&
         memcmp(a->cells, b->cells,
                (size_t)a->count * sizeof(a->cells[0])) == 0;
}

static int wait_for_panel_menu(struct screen_snapshot *screen,
                               int base, int focus_col, int hint_digit,
                               int visible) {
  int elapsed;
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(screen);
    if (panel_menu_is_exact(screen, base, focus_col, hint_digit, visible))
      return 1;
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

static void test_exact_panel_menus(void) {
  struct screen_snapshot baseline, menu, restored;
  usleep(300000);
  capture(&baseline);

  kviktest_send_key(KEY_F9);
  usleep(100000);
  kviktest_send_key(KEY_LEFT);
  kviktest_send_key(KEY_LEFT);
  kviktest_send_key(KEY_LEFT);
  kviktest_send_key(KEY_LEFT);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  check(wait_for_panel_menu(&menu, 2, 2, '1', 0),
        "4.05 hidden Left dropdown characters and attributes are exact");

  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&baseline, &restored),
        "4.05 Left cancellation restores every screen cell");

  kviktest_send_key(KEY_TAB);
  usleep(300000);
  capture(&baseline);
  kviktest_send_key(KEY_F9);
  usleep(100000);
  kviktest_send_key(KEY_END);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  check(wait_for_panel_menu(&menu, 42, 42, '2', 1),
        "4.05 Right dropdown characters and attributes are exact");

  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&baseline, &restored),
        "4.05 one-Escape cancellation restores every screen cell");
}

static void run_tests(void) {
  test_exact_panel_menus();
}

TEST_MAIN("test_menu_contract", "coverage_menu_contract.bin")
