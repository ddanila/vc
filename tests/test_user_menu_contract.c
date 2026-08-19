/* Exact VC 4.05 F2 User Menu character/attribute oracle. */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)
#define ATTR_HISTORY 0x3f
#define ATTR_HISTORY_CURSOR 0x0f

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

static int cells_equal_except_clock(const struct screen_snapshot *a,
                                    const struct screen_snapshot *b) {
  int row, col;
  if (a->count != SCREEN_CELLS || b->count != SCREEN_CELLS) return 0;
  for (row = 0; row < 25; ++row)
    for (col = 0; col < SCREEN_COLS; ++col) {
      int index = row * SCREEN_COLS + col;
      if (row == 0 && col >= 67) continue;
      if (a->cells[index] != b->cells[index]) return 0;
    }
  return 1;
}

static int key_bar_is_exact(const struct screen_snapshot *screen) {
  static const char text[] =
    "1Help   2       3       4       5       6       7       8       "
    "9       10Quit  ";
  int col;
  if (sizeof(text) - 1 != SCREEN_COLS) return 0;
  for (col = 0; col < SCREEN_COLS; ++col) {
    unsigned char expected_attr;
    if (col >= 72)
      expected_attr = col <= 73 ? 0x07 : 0x30;
    else
      expected_attr = col % 8 == 0 || col % 8 == 7 ? 0x07 : 0x30;
    if (cell_char(screen, 24, col) != (unsigned char)text[col] ||
        cell_attr(screen, 24, col) != expected_attr)
      return 0;
  }
  return 1;
}

static int shadow_is_exact(const struct screen_snapshot *screen,
                           const struct screen_snapshot *baseline) {
  int row, col;
  for (row = 6; row <= 10; ++row)
    for (col = 55; col <= 56; ++col)
      if (cell_char(screen, row, col) != cell_char(baseline, row, col) ||
          cell_attr(screen, row, col) !=
            ((cell_attr(baseline, row, col) & 0x8f) ^ 0x08))
        return 0;
  for (col = 27; col <= 56; ++col)
    if (cell_char(screen, 11, col) != cell_char(baseline, 11, col) ||
        cell_attr(screen, 11, col) !=
          ((cell_attr(baseline, 11, col) & 0x8f) ^ 0x08))
      return 0;
  return 1;
}

static int user_menu_is_exact(const struct screen_snapshot *screen,
                              const struct screen_snapshot *baseline,
                              int focused_row) {
  static const unsigned char *const box[] = {
    (const unsigned char *)"\xc9\xcd\xcd\xcd\xcd\xcd User Menu \xcd\xcd\xcd\xcd\xcd\xcd\xbb",
    (const unsigned char *)"\xba A  Assemble current  \xba",
    (const unsigned char *)"\xba V  View output       \xba",
    (const unsigned char *)"\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc"
  };
  int row, col;

  if (screen->count != SCREEN_CELLS || baseline->count != SCREEN_CELLS ||
      !key_bar_is_exact(screen) || !shadow_is_exact(screen, baseline))
    return 0;

  /* MinBox clears a fixed canvas before drawing the smaller menu frame. */
  for (row = 5; row <= 10; ++row)
    for (col = 25; col <= 54; ++col) {
      unsigned char expected_char = ' ';
      unsigned char expected_attr = ATTR_HISTORY;
      if (row >= 6 && row <= 9 && col >= 28 && col <= 51) {
        expected_char = box[row - 6][col - 28];
        if (row == focused_row && col >= 32 && col <= 49)
          expected_attr = ATTR_HISTORY_CURSOR;
      }
      if (cell_char(screen, row, col) != expected_char ||
          cell_attr(screen, row, col) != expected_attr)
        return 0;
    }

  /* No non-clock cell outside the canvas, shadow, or key bar may change. */
  for (row = 0; row < 24; ++row)
    for (col = 0; col < SCREEN_COLS; ++col) {
      int canvas = row >= 5 && row <= 10 && col >= 25 && col <= 54;
      int shadow = (row >= 6 && row <= 10 && col >= 55 && col <= 56) ||
                   (row == 11 && col >= 27 && col <= 56);
      int clock = row == 0 && col >= 67;
      int index = row * SCREEN_COLS + col;
      if (!canvas && !shadow && !clock &&
          screen->cells[index] != baseline->cells[index])
        return 0;
    }
  return 1;
}

static int wait_for_user_menu(struct screen_snapshot *screen,
                              const struct screen_snapshot *baseline,
                              int focused_row) {
  int elapsed;
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(screen);
    if (user_menu_is_exact(screen, baseline, focused_row)) return 1;
    usleep(10000);
  }
  return 0;
}

static int wait_for_restore(const struct screen_snapshot *baseline,
                            struct screen_snapshot *screen) {
  int elapsed;
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(screen);
    if (cells_equal_except_clock(baseline, screen)) return 1;
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
  check(wait_for_user_menu(&first, &baseline, 7),
        "4.05 User Menu initial characters and attributes are exact");

  kviktest_send_key(KEY_DOWN);
  check(wait_for_user_menu(&second, &baseline, 8),
        "4.05 User Menu Down focus characters and attributes are exact");

  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&baseline, &restored),
        "4.05 User Menu cancellation restores every non-clock screen cell");
}

static void run_tests(void) {
  test_exact_user_menu();
}

TEST_MAIN("test_user_menu_contract", "coverage_user_menu_contract.bin")
