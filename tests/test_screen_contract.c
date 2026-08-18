/*
 * test_screen_contract.c: exact VC 4.05 text-mode screen contract.
 *
 * Character-only substring checks cannot distinguish a correctly rendered
 * commander from a damaged frame or a cursor highlight on the wrong panel.
 * This test therefore checks both CP437 frame cells and VGA attributes.
 */
#include "test_common.h"

#define SCREEN_COLS 80
#define SCREEN_CELLS (SCREEN_COLS * 50)
#define ATTR_PANEL 0x1b
#define ATTR_HEADING 0x1e
#define ATTR_CURSOR 0x30
#define ATTR_DOS 0x07
#define KEY_HOME 0x4700

static int have_scroll_fixture;

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
  while (*text) {
    if (cell_char(screen, row, col++) != (unsigned char)*text++) return 0;
  }
  return 1;
}

static int panel_frame_is_exact(const struct screen_snapshot *screen,
                                int base) {
  int row, col;

  if (cell_char(screen, 0, base) != 0xc9 ||
      cell_char(screen, 0, base + 13) != 0xd1 ||
      cell_char(screen, 0, base + 26) != 0xd1 ||
      cell_char(screen, 0, base + 39) != 0xbb)
    return 0;
  for (col = 1; col <= 12; ++col)
    if (cell_char(screen, 0, base + col) != 0xcd) return 0;
  for (col = 14; col <= 16; ++col)
    if (cell_char(screen, 0, base + col) != 0xcd) return 0;
  if (!row_text_is(screen, 0, base + 17, " C:\\ ")) return 0;
  for (col = 22; col <= 25; ++col)
    if (cell_char(screen, 0, base + col) != 0xcd) return 0;
  for (col = 27; col <= 38; ++col)
    if (cell_char(screen, 0, base + col) != 0xcd) return 0;

  if (cell_char(screen, 1, base) != 0xba ||
      cell_char(screen, 1, base + 13) != 0xb3 ||
      cell_char(screen, 1, base + 26) != 0xb3 ||
      cell_char(screen, 1, base + 39) != 0xba ||
      !row_text_is(screen, 1, base + 5, "Name") ||
      !row_text_is(screen, 1, base + 18, "Name") ||
      !row_text_is(screen, 1, base + 31, "Name"))
    return 0;

  for (row = 2; row <= 19; ++row)
    if (cell_char(screen, row, base) != 0xba ||
        cell_char(screen, row, base + 13) != 0xb3 ||
        cell_char(screen, row, base + 26) != 0xb3 ||
        cell_char(screen, row, base + 39) != 0xba)
      return 0;

  if (cell_char(screen, 20, base) != 0xc7 ||
      cell_char(screen, 20, base + 13) != 0xc1 ||
      cell_char(screen, 20, base + 26) != 0xc1 ||
      cell_char(screen, 20, base + 39) != 0xb6)
    return 0;
  for (col = 1; col <= 38; ++col)
    if (col != 13 && col != 26 &&
        cell_char(screen, 20, base + col) != 0xc4)
      return 0;

  if (cell_char(screen, 21, base) != 0xba ||
      cell_char(screen, 21, base + 39) != 0xba ||
      cell_char(screen, 22, base) != 0xc8 ||
      cell_char(screen, 22, base + 39) != 0xbc)
    return 0;
  for (col = 1; col <= 38; ++col)
    if (cell_char(screen, 22, base + col) != 0xcd) return 0;
  return 1;
}

static int panel_attributes_are_exact(const struct screen_snapshot *screen,
                                      int base, int active) {
  int col;
  if (cell_attr(screen, 0, base) != ATTR_PANEL ||
      cell_attr(screen, 1, base) != ATTR_PANEL ||
      cell_attr(screen, 20, base) != ATTR_PANEL ||
      cell_attr(screen, 22, base + 39) != ATTR_PANEL)
    return 0;
  for (col = 0; col < 4; ++col) {
    if (cell_attr(screen, 1, base + 5 + col) != ATTR_HEADING ||
        cell_attr(screen, 1, base + 18 + col) != ATTR_HEADING ||
        cell_attr(screen, 1, base + 31 + col) != ATTR_HEADING)
      return 0;
  }
  for (col = 1; col <= 12; ++col)
    if (cell_attr(screen, 2, base + col) !=
        (active ? ATTR_CURSOR : ATTR_PANEL))
      return 0;
  for (col = 17; col <= 21; ++col)
    if (cell_attr(screen, 0, base + col) !=
        (active ? ATTR_CURSOR : ATTR_PANEL))
      return 0;
  return 1;
}

static int region_chars_equal(const struct screen_snapshot *a,
                              const struct screen_snapshot *b,
                              int first_row, int last_row) {
  int row, col;
  for (row = first_row; row <= last_row; ++row)
    for (col = 0; col < SCREEN_COLS; ++col)
      if (cell_char(a, row, col) != cell_char(b, row, col)) return 0;
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

static int row_attrs_are(const struct screen_snapshot *screen, int row,
                         int first_col, int last_col, unsigned char attr) {
  int col;
  for (col = first_col; col <= last_col; ++col)
    if (cell_attr(screen, row, col) != attr) return 0;
  return 1;
}

static int panel_frame_cells_equal(const struct screen_snapshot *a,
                                   const struct screen_snapshot *b,
                                   int base) {
  int row, col;
  for (col = base; col < base + 40; ++col)
    if (a->cells[col] != b->cells[col] ||
        a->cells[SCREEN_COLS + col] != b->cells[SCREEN_COLS + col] ||
        a->cells[20 * SCREEN_COLS + col] !=
          b->cells[20 * SCREEN_COLS + col] ||
        a->cells[22 * SCREEN_COLS + col] !=
          b->cells[22 * SCREEN_COLS + col])
      return 0;
  for (row = 2; row <= 19; ++row)
    for (col = 0; col < 4; ++col) {
      int frame_col = base + col * 13;
      if (a->cells[row * SCREEN_COLS + frame_col] !=
          b->cells[row * SCREEN_COLS + frame_col])
        return 0;
    }
  return a->cells[21 * SCREEN_COLS + base] ==
           b->cells[21 * SCREEN_COLS + base] &&
         a->cells[21 * SCREEN_COLS + base + 39] ==
           b->cells[21 * SCREEN_COLS + base + 39];
}

static int hidden_panel_is_dos_blank(const struct screen_snapshot *screen) {
  int row, col;
  for (row = 0; row <= 22; ++row)
    for (col = 0; col < 40; ++col)
      if (cell_char(screen, row, col) != ' ' ||
          cell_attr(screen, row, col) != ATTR_DOS)
        return 0;
  return 1;
}

static void run_tests(void) {
  struct screen_snapshot initial, both, idle, moved, moved_back;
  struct screen_snapshot hidden, restored, switched, typed;
  struct screen_snapshot before_scroll, scrolled;

  capture(&initial);
  check(initial.count == 25 * SCREEN_COLS, "captured all 25 text rows");
  check(hidden_panel_is_dos_blank(&initial),
        "initial inactive left panel is blank DOS text");
  check(panel_frame_is_exact(&initial, 40),
        "initial right panel has exact VC 4.05 frame and titles");
  check(panel_attributes_are_exact(&initial, 40, 1),
        "initial right panel has exact heading and cursor attributes");

  kviktest_send_key(0x1910);  /* Ctrl+P: show inactive left panel. */
  usleep(700000);
  capture(&both);
  check(panel_frame_is_exact(&both, 0) && panel_frame_is_exact(&both, 40),
        "Ctrl+P renders both exact panel frames");
  check(panel_attributes_are_exact(&both, 0, 0) &&
        panel_attributes_are_exact(&both, 40, 1),
        "inactive and active panel attributes are distinct and exact");

  usleep(1200000);
  capture(&idle);
  check(region_cells_equal(&both, &idle, 1, 24, 0, 79),
        "idle cursor/clock activity leaves all non-clock cells untouched");

  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  capture(&moved);
  check(region_cells_equal(&both, &moved, 1, 22, 0, 39),
        "in-page Down leaves every inactive-panel cell untouched");
  check(region_chars_equal(&both, &moved, 0, 20),
        "in-page Down leaves frames, headings, and file characters intact");
  check(row_attrs_are(&moved, 2, 41, 52, ATTR_PANEL) &&
        row_attrs_are(&moved, 3, 41, 52, ATTR_CURSOR),
        "in-page Down moves cursor attributes from old to new file");

  kviktest_send_key(KEY_UP);
  usleep(300000);
  capture(&moved_back);
  check(region_cells_equal(&both, &moved_back, 1, 22, 0, 79),
        "in-page Up restores all non-clock panel cells exactly");
  check(panel_attributes_are_exact(&moved_back, 0, 0) &&
        panel_attributes_are_exact(&moved_back, 40, 1),
        "in-page Up restores the original active cursor attributes");

  kviktest_send_key(0x180f);  /* Ctrl+O: temporarily hide both panels. */
  usleep(700000);
  capture(&hidden);
  check(!panel_frame_is_exact(&hidden, 0) &&
        !panel_frame_is_exact(&hidden, 40),
        "Ctrl+O removes both panel frames");
  check(row_text_is(&hidden, 23, 0, "C:\\>"),
        "Ctrl+O preserves the exact command prompt");

  kviktest_send_key(KEY_ESC);
  usleep(700000);
  capture(&restored);
  check(region_chars_equal(&both, &restored, 0, 24),
        "key after Ctrl+O restores every screen character");
  check(panel_attributes_are_exact(&restored, 0, 0) &&
        panel_attributes_are_exact(&restored, 40, 1),
        "key after Ctrl+O restores panel attributes");

  kviktest_send_key(KEY_TAB);
  usleep(500000);
  capture(&switched);
  check(region_chars_equal(&restored, &switched, 0, 24),
        "Tab changes active panel without damaging screen characters");
  check(panel_attributes_are_exact(&switched, 0, 1) &&
        panel_attributes_are_exact(&switched, 40, 0),
        "Tab moves the exact cursor attributes to the left panel");

  type_string("abc");
  usleep(300000);
  capture(&typed);
  check(row_text_is(&typed, 23, 0, "C:\\>abc"),
        "command entry appears at the exact prompt position");
  check(panel_frame_is_exact(&typed, 0) && panel_frame_is_exact(&typed, 40),
        "command entry preserves both exact panel frames");
  check(region_cells_equal(&switched, &typed, 1, 22, 0, 79),
        "command entry leaves every non-clock panel cell untouched");

  kviktest_send_key(KEY_ESC);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 2000),
        "Escape clears command entry back to the exact prompt");

  if (have_scroll_fixture) {
    kviktest_send_key(KEY_HOME);
    kviktest_send_key(KEY_RIGHT);
    kviktest_send_key(KEY_RIGHT);
    usleep(500000);
    capture(&before_scroll);
    kviktest_send_key(KEY_RIGHT);
    usleep(500000);
    capture(&scrolled);
    check(panel_frame_cells_equal(&before_scroll, &scrolled, 0),
          "page-boundary move leaves every active-panel frame cell intact");
    check(region_cells_equal(&before_scroll, &scrolled, 1, 22, 40, 79),
          "page-boundary move leaves inactive-panel cells untouched");
    check(region_cells_equal(&before_scroll, &scrolled, 23, 23, 0, 79),
          "page-boundary move leaves the command row untouched");
    check(!region_cells_equal(&before_scroll, &scrolled, 2, 21, 1, 38),
          "page-boundary move changes the active file/status interior");
  } else {
    check(1, "page-boundary contract skipped for caller-supplied fixture");
  }
}

static int add_scroll_files(const char *directory) {
  int index;
  char path[1024];
  for (index = 0; index < 60; ++index) {
    FILE *file;
    snprintf(path, sizeof(path), "%s/PF%02d.DAT", directory, index);
    file = fopen(path, "wb");
    if (!file) return -1;
    fputc('x', file);
    fclose(file);
  }
  return 0;
}

int main(int argc, char **argv) {
  char *fixture_dir = NULL;
  const char *mount_dir;
  if (!find_vc_com()) {
    fprintf(stderr, "SKIP: VC.COM not found\n"); return 0;
  }
  if (argc > 1) {
    mount_dir = argv[1];
  } else {
    if (setup_fixtures("tests/fixtures", &fixture_dir) != 0) {
      fprintf(stderr, "FAIL: could not create fixture dir\n"); return 1;
    }
    mount_dir = fixture_dir;
    if (add_scroll_files(mount_dir) != 0) {
      fprintf(stderr, "FAIL: could not create scroll fixtures\n");
      cleanup_fixtures(fixture_dir); return 1;
    }
    have_scroll_fixture = 1;
  }
  strncpy(g_mount_dir, mount_dir, sizeof(g_mount_dir) - 1);
  printf("=== test_screen_contract ===\n");
  printf("VC.COM: %s\n", vc_path);
  printf("mount:  %s\n", mount_dir);
  signal(SIGALRM, watchdog_handler);
  alarm(180);
  kviktest_coverage_enable();
  if (kviktest_start(vc_path, mount_dir) != 0) {
    fprintf(stderr, "FAIL: could not start kvikdos\n");
    cleanup_fixtures(fixture_dir); return 1;
  }
  if (!kviktest_wait_for_text(23, 0, "C:\\>", 15000)) {
    fprintf(stderr, "FAIL: VC.COM did not render prompt\n");
    kviktest_stop(); cleanup_fixtures(fixture_dir); return 1;
  }
  run_tests();
  alarm(0);
  kviktest_coverage_report(vc_path, 55296);
  kviktest_coverage_dump("coverage_screen_contract.bin");
  printf("\nStopping emulator...\n"); fflush(stdout);
  kviktest_stop();
  cleanup_fixtures(fixture_dir);
  printf("\n=== test_screen_contract: %d passed, %d failed ===\n",
         g_pass, g_fail);
  fflush(stdout);
  _exit(g_fail > 0 ? 1 : 0);
}
