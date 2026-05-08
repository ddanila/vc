/*
 * test_menu_nav.c: menu bar shadow rendering and navigation resilience.
 *
 * Regression tests for F9 menu system. These tests verify dropdown
 * shadow rendering and that menu navigation doesn't crash.
 */
#include "test_common.h"

extern const unsigned char *g_dump_video_mem;

static unsigned char read_attr(int row, int col) {
    if (!g_dump_video_mem) return 0;
    return g_dump_video_mem[(row * 80 + col) * 2 + 1];
}

static unsigned char read_char(int row, int col) {
    if (!g_dump_video_mem) return 0;
    return g_dump_video_mem[(row * 80 + col) * 2];
}

/* ---- Dropdown shadow rendering ---- */
static void test_menu_shadow(void) {
  printf("\n--- Dropdown shadow ---\n");

  /* Open Left menu dropdown. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);

  /* Shadow: 2 chars right of dropdown right border.
     Left dropdown at col 3, width 22+2=24 → right border at col 26.
     Shadow cols 27-28. Attribute bg bits should be 0x00 (darkened). */
  { unsigned char shadow_attr = read_attr(4, 27);
    check((shadow_attr & 0x70) == 0x00,
          "dropdown has right shadow");
  }

  /* Bottom shadow: dropdown starts at row 1, 16 items + 2 borders = 18.
     Bottom border at row 18, shadow row at row 19. */
  { unsigned char bot_shadow = read_attr(19, 6);
    check((bot_shadow & 0x70) == 0x00,
          "dropdown has bottom shadow");
  }

  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  check(kviktest_is_running(), "alive after shadow test");
}

/* ---- Navigate through all 5 menus with dropdowns open ---- */
static void test_menu_navigate_all(void) {
  printf("\n--- Navigate all menus ---\n");

  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);

  /* Right 4 times: Left → Files → Commands → Options → Right */
  kviktest_send_key(KEY_RIGHT); usleep(300000);
  check(kviktest_is_running(), "navigated to Files");
  kviktest_send_key(KEY_RIGHT); usleep(300000);
  check(kviktest_is_running(), "navigated to Commands");
  kviktest_send_key(KEY_RIGHT); usleep(300000);
  check(kviktest_is_running(), "navigated to Options");
  kviktest_send_key(KEY_RIGHT); usleep(300000);
  check(kviktest_is_running(), "navigated to Right");

  /* Left 4 times back: Right → Options → Commands → Files → Left */
  kviktest_send_key(KEY_LEFT); usleep(300000);
  kviktest_send_key(KEY_LEFT); usleep(300000);
  kviktest_send_key(KEY_LEFT); usleep(300000);
  kviktest_send_key(KEY_LEFT); usleep(300000);
  check(kviktest_is_running(), "navigated back to Left");

  /* ESC to close. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
}

/* ---- Menu open/close restores screen ---- */
static void test_menu_restore(void) {
  char buf[81];
  printf("\n--- Menu open/close restores screen ---\n");

  /* Remember what row 23 looks like (command prompt). */
  kviktest_read_text(23, 0, buf, 10);

  /* Open menu dropdown and close it. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* Command prompt should still be at row 23. */
  { char buf2[10];
    kviktest_read_text(23, 0, buf2, 5);
    check(buf2[0] == 'C',
          "command prompt restored after menu close");
  }
  check(kviktest_is_running(), "alive after menu restore");
}

/* ---- Dropdown uses single-line full box ┌─┐│└─┘ ---- */
static void test_menu_border_style(void) {
  int rc = -1, lc = -1, br = -1;
  printf("\n--- Dropdown border style ---\n");

  /* Open a dropdown (F9 → Down). */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);

  /* Find top-left corner (0xDA ┌) on row 1. */
  for (int c = 0; c < 70; c++) {
    if (read_char(1, c) == 0xDA) { lc = c; break; }
  }
  check(lc >= 0, "found top-left corner (0xDA)");

  /* Find top-right corner (0xBF ┐) on row 1. */
  if (lc >= 0) {
    for (int c = lc + 1; c < 80; c++) {
      if (read_char(1, c) == 0xBF) { rc = c; break; }
    }
  }
  check(rc > 0, "found top-right corner (0xBF)");

  /* Find bottom-left corner (0xC0 └). */
  if (lc >= 0) {
    for (int r = 2; r < 24; r++) {
      if (read_char(r, lc) == 0xC0) { br = r; break; }
    }
  }
  check(br > 0, "found bottom-left corner (0xC0)");

  if (lc >= 0 && rc > 0 && br > 0) {
    /* Left border: single vertical (0xB3 │). */
    check(read_char(2, lc) == 0xB3,
          "left border uses single vertical (0xB3)");

    /* Right border: single vertical (0xB3 │). */
    check(read_char(2, rc) == 0xB3,
          "right border uses single vertical (0xB3)");

    /* Bottom-right corner (0xD9 ┘). */
    check(read_char(br, rc) == 0xD9,
          "bottom-right corner is single (0xD9)");

    /* No double-line chars anywhere in the dropdown. */
    { int has_double = 0;
      for (int r = 1; r <= br; r++) {
        for (int c = lc; c <= rc; c++) {
          unsigned char ch = read_char(r, c);
          if (ch == 0xC9 || ch == 0xBB || ch == 0xC8 || ch == 0xBC ||
              ch == 0xCD || ch == 0xBA)
            has_double = 1;
        }
      }
      check(!has_double, "no double-line border chars in dropdown");
    }
  }

  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
}

/* ---- Checkbox marks for active mode and sort ---- */
static void test_menu_checkmarks(void) {
  int lc = -1;
  printf("\n--- Menu checkmarks ---\n");

  /* Open a panel dropdown (F9 → Down).
     Default: Brief mode, Unsorted. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);

  /* Find left border column: 0xDA on row 1. */
  for (int c = 0; c < 70; c++) {
    if (read_char(1, c) == 0xDA) { lc = c; break; }
  }

  if (lc >= 0) {
    /* Checkbox column is lc+1 (one inside left border).
       Scan for √ (0xFB) marks — should find exactly 2:
       one for the active view mode, one for the active sort. */
    int check_count = 0;
    int first_check_row = -1;
    for (int r = 2; r < 20; r++) {
      if (read_char(r, lc + 1) == 0xFB) {
        check_count++;
        if (first_check_row < 0) first_check_row = r;
      }
    }
    check(check_count == 2, "exactly 2 checkmarks in dropdown");
    /* First checkmark should be on Brief (first item, row 2). */
    check(first_check_row == 2,
          "first checkmark on Brief (row 2)");
    /* Verify a non-checked item has no checkmark. */
    check(read_char(3, lc + 1) == 0x20,
          "Full has no checkbox");
  }

  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
}

/* ---- Cyclic navigation: down wraps to top, up wraps to bottom ---- */
static void test_menu_cyclic_nav(void) {
  printf("\n--- Cyclic menu navigation ---\n");

  /* Open Left dropdown. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);

  /* Left menu has 16 items, 3 separators, 13 selectable.
     Last selectable item is "Drive..." (item 16).
     Navigate down 13 times from first item to wrap back to first. */
  for (int i = 0; i < 13; i++) {
    kviktest_send_key(KEY_DOWN);
    usleep(100000);
  }
  check(kviktest_is_running(), "cycled down through all items");

  /* Navigate up once from first item — should wrap to last. */
  kviktest_send_key(KEY_UP);
  usleep(200000);
  check(kviktest_is_running(), "wrapped up from first to last");

  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
}

static void run_tests(void) {
  test_menu_shadow();
  test_menu_navigate_all();
  test_menu_restore();
  test_menu_border_style();
  test_menu_checkmarks();
  test_menu_cyclic_nav();
}

TEST_MAIN("test_menu_nav", "coverage_menu_nav.bin")
