/* Exact VC 4.05 Full and Info panel oracle. */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (25 * SCREEN_COLS)
#define PANEL_BASE 40
#define PANEL_WIDTH 40
#define ATTR_PANEL 0x1b
#define ATTR_HEADING 0x1e
#define ATTR_CURSOR 0x30

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
}

TEST_MAIN("test_panel_metadata_contract", "coverage_panel_metadata_contract.bin")
