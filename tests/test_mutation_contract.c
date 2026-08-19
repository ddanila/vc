/*
 * Exact F6/F8/Ctrl-A contracts for the source-built VC 4.05.
 *
 * Broad file-operation tests prove DOS effects. This oracle pins the source
 * dialogs, attributes, current-versus-selected decision, and cancellation
 * restoration translated by VC/8080. It deliberately excludes 4.99 alpha.
 */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (SCREEN_COLS * 50)
#define ATTR_DIALOG 0x70
#define ATTR_EDITOR 0x30
#define ATTR_HEADING 0x7e

struct screen_snapshot {
  unsigned short cells[SCREEN_CELLS];
  int count;
};

struct attr_span {
  int row;
  int first_col;
  int last_col;
  unsigned char attr;
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

static int pattern_is(const struct screen_snapshot *screen, int row, int col,
                      const char *pattern, int width) {
  int index;
  if ((int)strlen(pattern) != width) return 0;
  for (index = 0; index < width; ++index)
    if (pattern[index] != '?' &&
        cell_char(screen, row, col + index) !=
          (unsigned char)pattern[index])
      return 0;
  return 1;
}

static int frame_chars_are_exact(const struct screen_snapshot *screen,
                                 int top, int left, int width, int height,
                                 const char *title,
                                 const char *const *interiors,
                                 int divider_row) {
  int row, col;
  int title_start = 1 + (width - 1 - (int)strlen(title)) / 2;
  if (cell_char(screen, top, left) != 0xc9 ||
      cell_char(screen, top, left + width - 1) != 0xbb ||
      cell_char(screen, top + height - 1, left) != 0xc8 ||
      cell_char(screen, top + height - 1, left + width - 1) != 0xbc)
    return 0;
  for (col = 1; col < width - 1; ++col) {
    unsigned char expected =
      col >= title_start && col < title_start + (int)strlen(title)
      ? (unsigned char)title[col - title_start] : 0xcd;
    if (cell_char(screen, top, left + col) != expected ||
        cell_char(screen, top + height - 1, left + col) != 0xcd)
      return 0;
  }
  for (row = 1; row < height - 1; ++row) {
    if (row == divider_row) {
      if (cell_char(screen, top + row, left) != 0xc7 ||
          cell_char(screen, top + row, left + width - 1) != 0xb6)
        return 0;
      for (col = 1; col < width - 1; ++col)
        if (cell_char(screen, top + row, left + col) != 0xc4)
          return 0;
    } else {
      if (cell_char(screen, top + row, left) != 0xba ||
          cell_char(screen, top + row, left + width - 1) != 0xba ||
          !pattern_is(screen, top + row, left + 1,
                      interiors[row - 1], width - 2))
        return 0;
    }
  }
  return 1;
}

static int frame_attrs_are_exact(const struct screen_snapshot *screen,
                                 int top, int left, int width, int height,
                                 const struct attr_span *spans,
                                 int span_count) {
  int row, col, index;
  for (row = top; row < top + height; ++row)
    for (col = left; col < left + width; ++col) {
      unsigned char expected = ATTR_DIALOG;
      for (index = 0; index < span_count; ++index)
        if (spans[index].row == row && col >= spans[index].first_col &&
            col <= spans[index].last_col) {
          expected = spans[index].attr;
          break;
        }
      if (cell_attr(screen, row, col) != expected)
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

static int rename_dialog_is_exact(const struct screen_snapshot *screen,
                                  int selected) {
  static const char *const single[] = {
    " Rename or move \"hello.txt\" to                                    ",
    " hello.txt                                                        ",
    "",
    "               [ Move ]   [ F10-Tree ]   [ Cancel ]               "
  };
  static const char *const group[] = {
    " Rename or move 2 files to                                        ",
    "                                                                  ",
    "",
    "               [ Move ]   [ F10-Tree ]   [ Cancel ]               "
  };
  static const struct attr_span attrs[] = {
    {8, 8, 71, ATTR_EDITOR}
  };
  return frame_chars_are_exact(screen, 6, 6, 68, 6, " Rename ",
                               selected ? group : single, 3) &&
         frame_attrs_are_exact(screen, 6, 6, 68, 6, attrs, 1);
}

static int delete_dialog_is_exact(const struct screen_snapshot *screen,
                                  int selected) {
  static const char *const single[] = {
    " Do you wish to delete ",
    "       hello.txt       ",
    "    Delete   Cancel    "
  };
  static const char *const group[] = {
    " You have selected ",
    "      2 files      ",
    "  Delete   Cancel  "
  };
  static const struct attr_span attrs[] = {
    {9, 31, 38, ATTR_EDITOR}
  };
  return frame_chars_are_exact(screen, 6, selected ? 29 : 27,
                               selected ? 21 : 25, 5, " Delete ",
                               selected ? group : single, -1) &&
         frame_attrs_are_exact(screen, 6, selected ? 29 : 27,
                               selected ? 21 : 25, 5, attrs, 1);
}

static int attributes_dialog_is_exact(const struct screen_snapshot *screen,
                                      int selected) {
  static const char *const single[] = {
    "  Change file attributes for  ",
    "         \"hello.txt\"          ",
    "  [ ] Read only    Date       ",
    "  [x] Archive      ?????????  ",
    "  [ ] Hidden       Time       ",
    "  [ ] System       ?????????  ",
    "",
    "     [ Set ]   [ Cancel ]     "
  };
  static const char *const group[] = {
    "       Change file attributes      ",
    "  Set  Clear                       ",
    "  [ ]  [ ] Read only    Date       ",
    "  [ ]  [ ] Archive                 ",
    "  [ ]  [ ] Hidden       Time       ",
    "  [ ]  [ ] System                  ",
    "",
    "        [ Set ]   [ Cancel ]       "
  };
  static const struct attr_span single_attrs[] = {
    {9, 44, 53, ATTR_EDITOR}, {11, 44, 53, ATTR_EDITOR}
  };
  static const struct attr_span group_attrs[] = {
    {7, 24, 33, ATTR_HEADING},
    {9, 46, 55, ATTR_EDITOR}, {11, 46, 55, ATTR_EDITOR}
  };
  int left = selected ? 21 : 24;
  int width = selected ? 37 : 32;
  return frame_chars_are_exact(screen, 5, left, width, 10, " Attributes ",
                               selected ? group : single, 7) &&
         frame_attrs_are_exact(screen, 5, left, width, 10,
                               selected ? group_attrs : single_attrs,
                               selected ? 3 : 2);
}

static int wait_for_dialog(
    struct screen_snapshot *screen,
    int (*dialog_check)(const struct screen_snapshot *, int),
    int selected) {
  int elapsed;
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(screen);
    if (dialog_check(screen, selected)) return 1;
    usleep(10000);
  }
  return 0;
}

static int wait_for_restore(const struct screen_snapshot *before,
                            struct screen_snapshot *restored) {
  int elapsed;
  for (elapsed = 0; elapsed < 3000; elapsed += 10) {
    capture(restored);
    if (cells_equal_except_clock(before, restored)) return 1;
    usleep(10000);
  }
  return 0;
}

static void check_cancel_restore(unsigned key,
                                 int (*dialog_check)(const struct screen_snapshot *,
                                                     int),
                                 const char *dialog_label,
                                 const char *restore_label,
                                 int selected) {
  struct screen_snapshot before, dialog, restored;
  capture(&before);
  kviktest_send_key(key);
  check(wait_for_dialog(&dialog, dialog_check, selected), dialog_label);
  kviktest_send_key(KEY_ESC);
  check(wait_for_restore(&before, &restored), restore_label);
}

static void run_tests(void) {
  check(navigate_to("hello", "HELLO"), "HELLO.TXT is current");
  check_cancel_restore(KEY_F6, rename_dialog_is_exact,
                       "F6 has exact single Rename dialog and attributes",
                       "F6 cancellation restores exact screen", 0);
  check_cancel_restore(KEY_F8, delete_dialog_is_exact,
                       "F8 has exact single Delete dialog and attributes",
                       "F8 cancellation restores exact screen", 0);
  check_cancel_restore(0x1e01, attributes_dialog_is_exact,
                       "Ctrl-A has exact single Attributes dialog",
                       "Ctrl-A cancellation restores exact screen", 0);

  kviktest_send_key(0x4700);  /* Home */
  kviktest_send_key(0x5200);  /* Insert */
  kviktest_send_key(0x5200);  /* Insert */
  usleep(500000);
  check_cancel_restore(KEY_F6, rename_dialog_is_exact,
                       "selected set has exact F6 dialog and attributes",
                       "selected F6 cancellation restores exact marks", 1);
  check_cancel_restore(KEY_F8, delete_dialog_is_exact,
                       "selected set has exact F8 dialog and attributes",
                       "selected F8 cancellation restores exact marks", 1);
  check_cancel_restore(0x1e01, attributes_dialog_is_exact,
                       "selected set has exact Ctrl-A dialog and attributes",
                       "selected Ctrl-A cancellation restores exact marks", 1);
}

TEST_MAIN("test_mutation_contract", "coverage_mutation_contract.bin")
