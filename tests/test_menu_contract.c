/* Exact VC 4.05 F9 menu character/attribute oracle. */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)

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

static int menu_bar_is_exact(const struct screen_snapshot *screen) {
  static const char bar[] =
    "    Left    Files    Commands    Options    Right";
  int col;

  if (screen->count != SCREEN_CELLS) return 0;
  for (col = 0; col < SCREEN_COLS; ++col) {
    unsigned char expected = col < (int)sizeof(bar) - 1 ?
                             (unsigned char)bar[col] : ' ';
    unsigned char attr = col >= 42 && col <= 50 ? 0x0f : 0x30;
    if (cell_char(screen, 0, col) != expected ||
        cell_attr(screen, 0, col) != attr)
      return 0;
  }
  return 1;
}

static int right_menu_is_exact(const struct screen_snapshot *screen) {
  static const unsigned char *const rows[] = {
    (const unsigned char *)"\xfb Brief                ",
    (const unsigned char *)"  Full                 ",
    (const unsigned char *)"  Info                 ",
    (const unsigned char *)"  Tree                 ",
    (const unsigned char *)"  On/Off       Ctrl-F2 ",
    NULL,
    (const unsigned char *)"  Name                 ",
    (const unsigned char *)"  eXtension            ",
    (const unsigned char *)"  tiMe                 ",
    (const unsigned char *)"  Size                 ",
    (const unsigned char *)"\xfb Unsorted             ",
    NULL,
    (const unsigned char *)"  Re-read      Ctrl-R  ",
    (const unsigned char *)"  fiLter...    Ctrl-F  ",
    (const unsigned char *)"  Drive...     Alt-F2  ",
  };
  static const unsigned char hot_col[] = {
    2, 2, 2, 2, 2, 0, 2, 3, 4, 2, 2, 0, 2, 4, 2
  };
  int row, col;

  if (!menu_bar_is_exact(screen)) return 0;

  if (cell_char(screen, 1, 42) != 0xda ||
      cell_char(screen, 1, 66) != 0xbf ||
      cell_char(screen, 17, 42) != 0xc0 ||
      cell_char(screen, 17, 66) != 0xd9)
    return 0;
  for (col = 43; col < 66; ++col) {
    if (cell_char(screen, 1, col) != 0xc4 ||
        cell_char(screen, 17, col) != 0xc4)
      return 0;
  }

  for (row = 0; row < 15; ++row) {
    int screen_row = row + 2;
    if (cell_char(screen, screen_row, 42) != 0xb3 ||
        cell_char(screen, screen_row, 66) != 0xb3 ||
        cell_attr(screen, screen_row, 42) != 0x30 ||
        cell_attr(screen, screen_row, 66) != 0x30)
      return 0;
    for (col = 0; col < 23; ++col) {
      unsigned char expected_char = rows[row] ? rows[row][col] :
                                              (col == 0 || col == 22 ?
                                               ' ' : 0xc4);
      unsigned char expected_attr = rows[row] ?
          (row == 0 ? 0x0f : 0x3f) : 0x30;
      if (rows[row] && col == hot_col[row])
        expected_attr = row == 0 ? 0x0e : 0x3e;
      if (cell_char(screen, screen_row, 43 + col) != expected_char ||
          cell_attr(screen, screen_row, 43 + col) != expected_attr)
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

static int wait_for_right_menu(struct screen_snapshot *screen) {
  int elapsed;
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(screen);
    if (right_menu_is_exact(screen)) return 1;
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

static void test_exact_right_menu(void) {
  struct screen_snapshot baseline, menu, restored;
  usleep(300000);
  capture(&baseline);

  kviktest_send_key(KEY_F9);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  check(wait_for_right_menu(&menu),
        "4.05 Right dropdown characters and attributes are exact");

  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&baseline, &restored),
        "4.05 one-Escape cancellation restores every screen cell");
}

static void run_tests(void) {
  test_exact_right_menu();
}

TEST_MAIN("test_menu_contract", "coverage_menu_contract.bin")
