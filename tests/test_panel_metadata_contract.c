/* Exact VC 4.05 Full and Info panel oracle. */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)
#define PANEL_BASE 40
#define PANEL_WIDTH 40
#define ATTR_PANEL 0x1b
#define ATTR_HEADING 0x1e
#define ATTR_CURSOR 0x30
#define ATTR_SELECTED 0x1e

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

static int panel_chars_are(const struct screen_snapshot *screen,
                           const unsigned char *const rows[23]) {
  int row, col;
  if (screen->count != SCREEN_CELLS) return 0;
  for (row = 0; row <= 22; ++row) {
    if (strlen((const char *)rows[row]) != PANEL_WIDTH) return 0;
    for (col = 0; col < PANEL_WIDTH; ++col)
      if (cell_char(screen, row, PANEL_BASE + col) != rows[row][col])
        return 0;
  }
  return 1;
}

static void report_char_mismatch(const struct screen_snapshot *screen,
                                 const unsigned char *const rows[23],
                                 const char *mode) {
  int row, col;
  for (row = 0; row <= 22; ++row) {
    size_t length = strlen((const char *)rows[row]);
    if (length != PANEL_WIDTH) {
      fprintf(stderr, "%s row %d expected length %lu, not %d\n",
              mode, row, (unsigned long)length, PANEL_WIDTH);
      return;
    }
    for (col = 0; col < PANEL_WIDTH; ++col)
      if (cell_char(screen, row, PANEL_BASE + col) != rows[row][col]) {
        fprintf(stderr, "%s row %d col %d expected %02x, got %02x\n",
                mode, row, col, rows[row][col],
                cell_char(screen, row, PANEL_BASE + col));
        return;
      }
  }
}

static int unchanged_outside_panel(const struct screen_snapshot *before,
                                   const struct screen_snapshot *after) {
  int row, col;
  for (row = 0; row <= 22; ++row)
    for (col = 0; col < PANEL_BASE; ++col)
      if (before->cells[row * SCREEN_COLS + col] !=
          after->cells[row * SCREEN_COLS + col])
        return 0;
  for (row = 23; row <= 24; ++row)
    for (col = 0; col < SCREEN_COLS; ++col)
      if (before->cells[row * SCREEN_COLS + col] !=
          after->cells[row * SCREEN_COLS + col])
        return 0;
  return 1;
}

static void open_right_panel_menu(void) {
  kviktest_send_key(KEY_F9);
  usleep(100000);
  kviktest_send_key(0x4f00);  /* End: Right menu. */
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  usleep(100000);
}

static const unsigned char *const full_rows[23] = {
  (const unsigned char *)"\xc9\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xd1\xcd\xcd\xcd C:\\ \xcd\xd1\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xd1\xcd\xcd\xcd\xcd\xcd\xcd\xbb",
  (const unsigned char *)"\xba    Name    \xb3   Size  \xb3  Date  \xb3 Time \xba",
  (const unsigned char *)"\xbazzz      bat\xb3       30\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbazebra    txt\xb3       18\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbavc       hlp\xb3    68836\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbavc       ext\xb3       94\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbatest     bat\xb3       10\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbaTEP      BIN\xb3\x10SUB-DIR\x11\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbareadme   txt\xb3       21\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbamiddle   dat\xb3        1\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbahello    txt\xb3       12\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xbagamma    txt\xb3       16\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xba" "dirinfo     \xb3       61\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xba" "delta    bin\xb3        3\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xba" "data     bin\xb3        4\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xba" "beta     doc\xb3        2\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xba" "alpha    doc\xb3        5\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xba" "aaa      com\xb3        5\xb3 8-18-26\xb3 7:00p\xba",
  (const unsigned char *)"\xba            \xb3         \xb3        \xb3      \xba",
  (const unsigned char *)"\xba            \xb3         \xb3        \xb3      \xba",
  (const unsigned char *)"\xc7\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xb6",
  (const unsigned char *)"\xbazzz.bat             30  8-18-26  7:00p\xba",
  (const unsigned char *)"\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc",
};

static int full_attrs_are_exact(const struct screen_snapshot *screen) {
  int row, col;
  for (row = 0; row <= 22; ++row)
    for (col = 0; col < PANEL_WIDTH; ++col) {
      unsigned char expected = ATTR_PANEL;
      if (row == 0 && col >= 17 && col <= 21) expected = ATTR_CURSOR;
      if (row == 1 && ((col >= 5 && col <= 12) ||
                       (col >= 14 && col <= 22) ||
                       (col >= 24 && col <= 31) ||
                       (col >= 33 && col <= 37)))
        expected = ATTR_HEADING;
      if (row == 2 && col >= 1 && col <= 38) expected = ATTR_CURSOR;
      if (cell_attr(screen, row, PANEL_BASE + col) != expected) return 0;
    }
  return 1;
}

static const unsigned char *const info_rows[23] = {
  (const unsigned char *)"\xc9"
    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd Info "
    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbb",
  (const unsigned char *)"\xba  The Volkov Commander, Version 4.05  \xba",
  (const unsigned char *)"\xc7\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb6",
  (const unsigned char *)"\xba         655,360 Bytes Memory         \xba",
  (const unsigned char *)"\xba          638,528 Bytes Free          \xba",
  (const unsigned char *)"\xba   Volume in drive C: has no label    \xba",
  (const unsigned char *)"\xba 268,431,360 total bytes on drive C:  \xba",
  (const unsigned char *)"\xba  268,431,360 bytes free on drive C:  \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xc7\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb6",
  (const unsigned char *)"\xba Test fixture directory               \xba",
  (const unsigned char *)"\xba Contains files for VC.COM e2e tests  \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xba                                      \xba",
  (const unsigned char *)"\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc",
};

static int info_attrs_are_exact(const struct screen_snapshot *screen) {
  int row, col;
  for (row = 0; row <= 22; ++row)
    for (col = 0; col < PANEL_WIDTH; ++col) {
      unsigned char expected = ATTR_PANEL;
      if (row == 0 && col >= 17 && col <= 22) expected = ATTR_CURSOR;
      if ((row == 3 && col >= 10 && col <= 16) ||
          (row == 4 && col >= 11 && col <= 17) ||
          (row == 5 && col >= 20 && col <= 21) ||
          (row == 6 && ((col >= 2 && col <= 12) ||
                        (col >= 35 && col <= 36))) ||
          (row == 7 && ((col >= 3 && col <= 13) ||
                        (col >= 35 && col <= 36))))
        expected = ATTR_HEADING;
      if (cell_attr(screen, row, PANEL_BASE + col) != expected) return 0;
    }
  return 1;
}

static int wait_for_mode(struct screen_snapshot *screen, int info) {
  int elapsed;
  for (elapsed = 0; elapsed < 5000; elapsed += 20) {
    capture(screen);
    if (panel_chars_are(screen, info ? info_rows : full_rows) &&
        (info ? info_attrs_are_exact(screen) : full_attrs_are_exact(screen)))
      return 1;
    usleep(20000);
  }
  return 0;
}

static int brief_order_is(const struct screen_snapshot *screen,
                          const char *const names[17], int cursor_row) {
  int row, col;
  if (screen->count != SCREEN_CELLS) return 0;
  for (row = 0; row < 17; ++row) {
    if (strlen(names[row]) != 12) return 0;
    for (col = 0; col < 12; ++col) {
      unsigned char expected_attr = row + 2 == cursor_row ?
                                    ATTR_CURSOR : ATTR_PANEL;
      if (cell_char(screen, row + 2, 41 + col) !=
            (unsigned char)names[row][col] ||
          cell_attr(screen, row + 2, 41 + col) != expected_attr)
        return 0;
    }
    for (col = 54; col <= 78; ++col) {
      unsigned char expected_char = col == 66 ? 0xb3 : ' ';
      if (cell_char(screen, row + 2, col) != expected_char ||
          cell_attr(screen, row + 2, col) != ATTR_PANEL)
        return 0;
    }
  }
  for (col = 41; col <= 78; ++col)
    if (cell_char(screen, 19, col) !=
          (col == 53 || col == 66 ? 0xb3 : ' ') ||
        cell_attr(screen, 19, col) != ATTR_PANEL)
      return 0;
  return cell_char(screen, 1, 45) == 'N' &&
         cell_char(screen, 1, 58) == 'N' &&
         cell_char(screen, 1, 71) == 'N' &&
         cell_char(screen, 21, 41) == 'z' &&
         cell_char(screen, 21, 42) == 'z' &&
         cell_char(screen, 21, 43) == 'z' &&
         cell_char(screen, 21, 44) == '.' &&
         cell_char(screen, 21, 45) == 'b' &&
         cell_char(screen, 21, 46) == 'a' &&
         cell_char(screen, 21, 47) == 't';
}

static int wait_for_brief_order(struct screen_snapshot *screen,
                                const char *const names[17],
                                int cursor_row) {
  int elapsed;
  for (elapsed = 0; elapsed < 5000; elapsed += 20) {
    capture(screen);
    if (brief_order_is(screen, names, cursor_row)) return 1;
    usleep(20000);
  }
  return 0;
}

static int sort_checkmark_is_exact(const struct screen_snapshot *screen,
                                   int sort_row) {
  int row, count = 0;
  for (row = 2; row <= 16; ++row)
    if (cell_char(screen, row, 43) == 0xfb) {
      ++count;
      if (row != 2 && row != sort_row) return 0;
    }
  return count == 2 && cell_char(screen, 2, 43) == 0xfb &&
         cell_char(screen, sort_row, 43) == 0xfb;
}

static int screens_equal(const struct screen_snapshot *a,
                         const struct screen_snapshot *b) {
  return a->count == b->count &&
         memcmp(a->cells, b->cells,
                (size_t)a->count * sizeof(a->cells[0])) == 0;
}

static void check_sort(const char *hotkey, const char *label,
                       const char *const names[17], int cursor_row,
                       int sort_row) {
  struct screen_snapshot sorted, menu, restored;
  char message[128];

  open_right_panel_menu();
  type_string(hotkey);
  snprintf(message, sizeof(message),
           "4.05 %s sort has exact order and preserves the cursor", label);
  check(wait_for_brief_order(&sorted, names, cursor_row), message);

  open_right_panel_menu();
  usleep(200000);
  capture(&menu);
  snprintf(message, sizeof(message),
           "4.05 %s sort has the exact menu checkmark", label);
  check(sort_checkmark_is_exact(&menu, sort_row), message);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  capture(&restored);
  snprintf(message, sizeof(message),
           "4.05 %s menu cancellation restores every cell", label);
  check(screens_equal(&sorted, &restored), message);
}

static const char *const name_order[17] = {
  "TEP      BIN", "aaa      com", "alpha    doc", "beta     doc",
  "data     bin", "delta    bin", "dirinfo     ", "gamma    txt",
  "hello    txt", "middle   dat", "readme   txt", "test     bat",
  "vc       ext", "vc       hlp", "vc       ini", "zebra    txt",
  "zzz      bat",
};

static const char *const extension_order[17] = {
  "TEP      BIN", "dirinfo     ", "test     bat", "zzz      bat",
  "data     bin", "delta    bin", "aaa      com", "middle   dat",
  "alpha    doc", "beta     doc", "vc       ext", "vc       hlp",
  "vc       ini", "gamma    txt", "hello    txt", "readme   txt",
  "zebra    txt",
};

static const char *const time_order[17] = {
  "TEP      BIN", "vc       ini", "aaa      com", "alpha    doc",
  "beta     doc", "data     bin", "delta    bin", "dirinfo     ",
  "gamma    txt", "hello    txt", "middle   dat", "readme   txt",
  "test     bat", "vc       ext", "vc       hlp", "zebra    txt",
  "zzz      bat",
};

static const char *const size_order[17] = {
  "TEP      BIN", "vc       hlp", "vc       ini", "vc       ext",
  "dirinfo     ", "zzz      bat", "readme   txt", "zebra    txt",
  "gamma    txt", "hello    txt", "test     bat", "aaa      com",
  "alpha    doc", "data     bin", "delta    bin", "beta     doc",
  "middle   dat",
};

static const char *const unsorted_order[17] = {
  "vc       ini", "zzz      bat", "zebra    txt", "vc       hlp",
  "vc       ext", "test     bat", "TEP      BIN", "readme   txt",
  "middle   dat", "hello    txt", "gamma    txt", "dirinfo     ",
  "delta    bin", "data     bin", "beta     doc", "alpha    doc",
  "aaa      com",
};

static int compare_dialog_is_exact(const struct screen_snapshot *screen) {
  static const unsigned char *const rows[5] = {
    (const unsigned char *)"\xc9\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd Compare \xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbb",
    (const unsigned char *)"\xba The two directories appear \xba",
    (const unsigned char *)"\xba      to be identical.      \xba",
    (const unsigned char *)"\xba             Ok             \xba",
    (const unsigned char *)"\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc",
  };
  int row, col;
  for (row = 0; row < 5; ++row) {
    if (strlen((const char *)rows[row]) != 30) return 0;
    for (col = 0; col < 30; ++col) {
      unsigned char expected_attr =
          row == 3 && col >= 13 && col <= 16 ? ATTR_CURSOR : 0x70;
      if (cell_char(screen, 8 + row, 25 + col) != rows[row][col] ||
          cell_attr(screen, 8 + row, 25 + col) != expected_attr)
        return 0;
    }
  }
  for (row = 8; row <= 12; ++row)
    if (cell_attr(screen, row, 58) != 0x03 ||
        cell_attr(screen, row, 59) != 0x03)
      return 0;
  return 1;
}

static int wait_for_compare_dialog(struct screen_snapshot *screen) {
  int elapsed;
  for (elapsed = 0; elapsed < 3000; elapsed += 20) {
    capture(screen);
    if (compare_dialog_is_exact(screen)) return 1;
    usleep(20000);
  }
  return 0;
}

static int write_compare_file(const char *directory, const char *name,
                              const char *contents) {
  char path[1024];
  FILE *file;
  snprintf(path, sizeof(path), "%s/%s", directory, name);
  file = fopen(path, "wb");
  if (!file) return 0;
  if (fputs(contents, file) == EOF || fclose(file) != 0) return 0;
  return 1;
}

static int chars_equal_except_status(const struct screen_snapshot *before,
                                     const struct screen_snapshot *after) {
  int row, col;
  for (row = 0; row < 25; ++row) {
    if (row == 21) continue;
    for (col = 0; col < SCREEN_COLS; ++col)
      if (cell_char(before, row, col) != cell_char(after, row, col)) return 0;
  }
  return 1;
}

static int row_chars_are(const struct screen_snapshot *screen, int row,
                         int col, const char *text) {
  while (*text)
    if (cell_char(screen, row, col++) != (unsigned char)*text++) return 0;
  return 1;
}

static int name_attrs_are(const struct screen_snapshot *screen, int row,
                          int col, unsigned char attr) {
  int offset;
  for (offset = 0; offset < 12; ++offset)
    if (cell_attr(screen, row, col + offset) != attr) return 0;
  return 1;
}

static int different_compare_is_exact(const struct screen_snapshot *screen) {
  int row;
  static const char left_status[] =
      "  69,636 bytes in 17 selected files   ";
  static const char right_status[] =
      "      9 bytes in 1 selected file      ";

  if (!row_chars_are(screen, 0, 54, " C:\\CMPDIR\\ ") ||
      !row_chars_are(screen, 21, 1, left_status) ||
      !row_chars_are(screen, 21, 41, right_status) ||
      !name_attrs_are(screen, 2, 1, ATTR_SELECTED) ||
      !name_attrs_are(screen, 3, 1, ATTR_PANEL) ||
      !name_attrs_are(screen, 10, 1, ATTR_PANEL) ||
      !name_attrs_are(screen, 2, 14, ATTR_SELECTED) ||
      !name_attrs_are(screen, 2, 41, ATTR_CURSOR) ||
      !name_attrs_are(screen, 3, 41, ATTR_SELECTED))
    return 0;
  for (row = 4; row <= 19; ++row)
    if (row != 10 && !name_attrs_are(screen, row, 1, ATTR_SELECTED))
      return 0;
  for (row = 3; row <= 19; ++row)
    if (row != 3 && !name_attrs_are(screen, row, 41, ATTR_PANEL))
      return 0;
  return 1;
}

static void run_tests(void) {
  struct screen_snapshot brief, full, info;
  usleep(500000);
  capture(&brief);

  open_right_panel_menu();
  type_string("f");
  {
    int exact = wait_for_mode(&full, 0);
    if (!exact) {
      report_char_mismatch(&full, full_rows, "Full");
      fprintf(stderr, "Full components: chars=%d attrs=%d\n",
              panel_chars_are(&full, full_rows), full_attrs_are_exact(&full));
    }
    check(exact, "4.05 Full panel characters and attributes are exact");
  }
  check(unchanged_outside_panel(&brief, &full),
        "Full mode changes only the selected panel");

  open_right_panel_menu();
  type_string("i");
  {
    int exact = wait_for_mode(&info, 1);
    if (!exact) {
      report_char_mismatch(&info, info_rows, "Info");
      fprintf(stderr, "Info components: chars=%d attrs=%d\n",
              panel_chars_are(&info, info_rows), info_attrs_are_exact(&info));
    }
    check(exact,
          "4.05 Info panel characters and highlighted fields are exact");
  }
  check(unchanged_outside_panel(&full, &info),
        "Info mode changes only the selected panel");

  open_right_panel_menu();
  type_string("b");
  usleep(700000);
  check(host_path_exists("VC.INI"),
        "4.05 setup file exists before the sort oracle");
  kviktest_send_key(0x1312);  /* Ctrl+R: pin the unsorted discovery order. */
  usleep(700000);
  check(navigate_to("zzz.bat", "ZZZ.BAT"),
        "sort oracle starts with ZZZ.BAT focused");

  check_sort("n", "Name", name_order, 18, 8);
  check_sort("x", "Extension", extension_order, 5, 9);
  check_sort("m", "Time", time_order, 18, 10);
  check_sort("s", "Size", size_order, 7, 11);
  check_sort("u", "Unsorted", unsorted_order, 3, 12);

  kviktest_send_key(0x1910);  /* Ctrl+P: show the left panel. */
  usleep(500000);
  kviktest_send_key(0x1312);  /* Ctrl+R: refresh right. */
  usleep(500000);
  kviktest_send_key(KEY_TAB);
  kviktest_send_key(0x1312);  /* Ctrl+R: refresh left. */
  usleep(500000);
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  {
    struct screen_snapshot before_compare, dialog, restored;
    capture(&before_compare);
    kviktest_send_key(0x2e03);  /* Ctrl+C: compare directories. */
    check(wait_for_compare_dialog(&dialog),
          "4.05 identical-directory Compare dialog is exact");
    kviktest_send_key(KEY_ESC);
    usleep(300000);
    capture(&restored);
    check(screens_equal(&before_compare, &restored),
          "Compare acknowledgement restores every screen cell");
  }

  {
    char compare_dir[1024];
    struct screen_snapshot before_different, compared;
    snprintf(compare_dir, sizeof(compare_dir), "%s/CMPDIR", g_mount_dir);
    check(mkdir(compare_dir, 0700) == 0,
          "created isolated compare directory");
    check(write_compare_file(compare_dir, "ONLYSUB.TXT", "sub-nine\n"),
          "created nine-byte subdirectory compare fixture");
    check(write_compare_file(g_mount_dir, "ONLYROOT.TXT", "root-7\n"),
          "created seven-byte root compare fixture");

    kviktest_send_key(0x1312);  /* Refresh right root panel. */
    usleep(500000);
    kviktest_send_key(KEY_TAB);
    kviktest_send_key(0x1312);  /* Refresh left root panel. */
    usleep(500000);
    kviktest_send_key(KEY_TAB);
    check(navigate_to("CMPDIR", "cmpdir"),
          "focused compare directory in the right panel");
    kviktest_send_key(0x7600);  /* Ctrl+PgDn: enter directory. */
    usleep(700000);
    capture(&before_different);
    kviktest_send_key(0x2e03);  /* Ctrl+C: compare different directories. */
    usleep(700000);
    capture(&compared);
    check(chars_equal_except_status(&before_different, &compared),
          "different-directory Compare changes only selection state");
    check(different_compare_is_exact(&compared),
          "different-directory Compare marks exact files and byte totals");
  }
}

TEST_MAIN("test_panel_metadata_contract", "coverage_panel_metadata_contract.bin")
