/*
 * test_nav_edges.c: exercise navigation edge cases in help, tree, hex, and panels.
 *
 * Targets untested paths in:
 *   - show_help: PgDn/PgUp/Home/End in content pages, Shift+Tab
 *   - tree_dialog: PgDn/PgUp/Home/End, Enter to select
 *   - render_hex_ascii_view: search in hex, tiny file
 *   - render_file_panel: Full mode cursor navigation
 */
#include "test_common.h"

#define KEY_PGDN      0x5100
#define KEY_PGUP      0x4900
#define KEY_HOME      0x4700
#define KEY_END       0x4F00
#define KEY_DOWN      0x5000
#define KEY_UP        0x4800
/* KEY_TAB already defined in test_harness.h */
#define KEY_SHIFT_TAB 0x0F00
#define KEY_BACKSPACE 0x0E08
#define KEY_ALT_F10   0x7100
#define KEY_CTRL_P    0x1910

/* ---- Help: PgDn/PgUp in content page ---- */
static void test_help_pgdn_pgup(void) {
  printf("\n--- Help PgDn/PgUp ---\n");

  kviktest_send_key(KEY_F1);
  usleep(1000000);

  /* Navigate to a content page: Tab to select a link, Enter to follow. */
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  /* Page down several times. */
  kviktest_send_key(KEY_PGDN);
  usleep(300000);
  kviktest_send_key(KEY_PGDN);
  usleep(300000);
  kviktest_send_key(KEY_PGDN);
  usleep(300000);

  check(kviktest_is_running(), "alive after PgDn in help");

  /* Page up back. */
  kviktest_send_key(KEY_PGUP);
  usleep(300000);
  kviktest_send_key(KEY_PGUP);
  usleep(300000);
  kviktest_send_key(KEY_PGUP);
  usleep(300000);

  check(kviktest_is_running(), "alive after PgUp in help");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
}

/* ---- Help: Home/End in content page ---- */
static void test_help_home_end(void) {
  printf("\n--- Help Home/End ---\n");

  kviktest_send_key(KEY_F1);
  usleep(1000000);

  /* Enter a content page. */
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  kviktest_send_key(KEY_END);
  usleep(300000);

  check(kviktest_is_running(), "alive after End in help");

  kviktest_send_key(KEY_HOME);
  usleep(300000);

  check(kviktest_is_running(), "alive after Home in help");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
}

/* ---- Help: Shift+Tab reverse link navigation ---- */
static void test_help_shift_tab(void) {
  printf("\n--- Help Shift+Tab ---\n");

  kviktest_send_key(KEY_F1);
  usleep(1000000);

  /* Tab forward through links. */
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_TAB);
  usleep(300000);

  /* Shift+Tab back. */
  kviktest_send_key(KEY_SHIFT_TAB);
  usleep(300000);
  kviktest_send_key(KEY_SHIFT_TAB);
  usleep(300000);

  check(kviktest_is_running(), "alive after Shift+Tab in help");

  /* Follow the current link. */
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  check(kviktest_is_running(), "alive after Enter on shifted link");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
}

/* ---- Help: deep page traversal ---- */
static void test_help_deep_traverse(void) {
  printf("\n--- Help deep traversal ---\n");

  kviktest_send_key(KEY_F1);
  usleep(1000000);

  /* Follow 4 links deep with PgDn on each page. */
  for (int i = 0; i < 4; i++) {
    kviktest_send_key(KEY_TAB);
    usleep(300000);
    kviktest_send_key(KEY_ENTER);
    usleep(500000);
    kviktest_send_key(KEY_PGDN);
    usleep(300000);
  }

  check(kviktest_is_running(), "alive after deep help traversal");

  /* Backspace 4 times to return through history. */
  for (int i = 0; i < 4; i++) {
    kviktest_send_key(KEY_BACKSPACE);
    usleep(500000);
  }

  check(kviktest_is_running(), "alive after backspace history");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
}

/* ---- Tree dialog: PgDn/PgUp/Home/End ---- */
static void test_tree_pgdn_pgup(void) {
  printf("\n--- Tree dialog PgDn/PgUp ---\n");

  /* Alt+F10 to open directory tree dialog. */
  kviktest_send_key(KEY_ALT_F10);
  usleep(2000000);  /* Tree scan takes time */

  kviktest_send_key(KEY_PGDN);
  usleep(300000);
  kviktest_send_key(KEY_PGUP);
  usleep(300000);
  kviktest_send_key(KEY_HOME);
  usleep(300000);
  kviktest_send_key(KEY_END);
  usleep(300000);

  check(kviktest_is_running(), "alive after tree dialog navigation");

  kviktest_send_key(KEY_ESC);
  usleep(500000);

  check(kviktest_is_running(), "back to panels after tree dialog");
}

/* ---- Tree dialog: Enter to select directory ---- */
static void test_tree_enter_select(void) {
  printf("\n--- Tree dialog Enter select ---\n");

  kviktest_send_key(KEY_ALT_F10);
  usleep(2000000);

  /* Navigate down to find a subdirectory. */
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);

  /* Enter to select the directory and return to panels. */
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  check(kviktest_is_running(), "alive after tree Enter select");

  /* Return to root. */
  kviktest_send_key(0x2B1C); /* Ctrl+\ */
  usleep(1000000);
}

/* ---- Hex viewer: F7 search ---- */
static void test_hex_search(void) {
  printf("\n--- Hex viewer search ---\n");

  navigate_to("data", "DATA");
  kviktest_send_key(KEY_F3);
  usleep(1000000);

  /* Switch to hex mode. */
  kviktest_send_key(KEY_F4);
  usleep(500000);

  /* F7 search in hex mode. */
  kviktest_send_key(KEY_F7);
  usleep(500000);

  type_string("test");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  check(kviktest_is_running(), "alive after hex search");

  /* Dismiss any not-found dialog. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* Exit viewer. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
}

/* ---- Hex viewer: tiny file (MIDDLE.DAT = 1 byte) ---- */
static void test_hex_tiny_file(void) {
  printf("\n--- Hex tiny file ---\n");

  navigate_to("middle", "MIDDLE");
  kviktest_send_key(KEY_F3);
  usleep(1000000);

  /* Switch to hex mode. */
  kviktest_send_key(KEY_F4);
  usleep(500000);

  /* Try all navigation keys — should be no-ops or clamped. */
  kviktest_send_key(KEY_HOME);
  usleep(200000);
  kviktest_send_key(KEY_END);
  usleep(200000);
  kviktest_send_key(KEY_PGDN);
  usleep(200000);
  kviktest_send_key(KEY_PGUP);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_UP);
  usleep(200000);

  check(kviktest_is_running(), "alive after hex tiny file nav");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
}

/* ---- Full mode: cursor navigation ---- */
static void test_full_mode_navigate(void) {
  printf("\n--- Full mode navigation ---\n");

  /* Switch to Full mode via F9 → Right → Full. */
  kviktest_send_key(KEY_F9);
  usleep(500000);
  kviktest_send_key(0x4D00); /* Right x4 to "Right" menu */
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(KEY_ENTER); /* Open dropdown */
  usleep(300000);
  kviktest_send_key(KEY_DOWN); /* Down to Full */
  usleep(200000);
  kviktest_send_key(KEY_ENTER); /* Select Full */
  usleep(500000);

  /* Navigate extensively in Full mode. */
  kviktest_send_key(KEY_HOME);
  usleep(200000);
  for (int i = 0; i < 5; i++) {
    kviktest_send_key(KEY_DOWN);
    usleep(150000);
  }
  kviktest_send_key(KEY_PGDN);
  usleep(300000);
  kviktest_send_key(KEY_PGUP);
  usleep(300000);
  kviktest_send_key(KEY_END);
  usleep(300000);

  check(kviktest_is_running(), "alive after Full mode navigation");

  /* Switch back to Brief. */
  kviktest_send_key(KEY_F9);
  usleep(500000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(300000);
  kviktest_send_key(KEY_ENTER); /* Brief (first item) */
  usleep(500000);
}

/* ---- Full mode: both panels ---- */
static void test_full_mode_both_panels(void) {
  printf("\n--- Full mode both panels ---\n");

  /* Enable left panel. */
  kviktest_send_key(KEY_CTRL_P);
  usleep(500000);

  /* Set right panel to Full via menu. */
  kviktest_send_key(KEY_F9);
  usleep(500000);
  kviktest_send_key(0x4D00); /* Right x4 */
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(300000);
  kviktest_send_key(KEY_DOWN); /* Full */
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  /* Tab to left panel, set it to Full. */
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_F9);
  usleep(500000);
  kviktest_send_key(KEY_ENTER); /* Left menu (first) */
  usleep(300000);
  kviktest_send_key(KEY_DOWN); /* Full */
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  /* Navigate in left panel. */
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);

  /* Tab to right panel and navigate. */
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);

  check(kviktest_is_running(), "alive after both panels Full mode");

  /* Restore: left Brief, right Brief, hide left panel. */
  kviktest_send_key(KEY_TAB); /* back to left */
  usleep(200000);
  kviktest_send_key(KEY_F9);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(300000);
  kviktest_send_key(KEY_ENTER); /* Brief */
  usleep(500000);
  kviktest_send_key(KEY_TAB); /* to right */
  usleep(200000);
  kviktest_send_key(KEY_F9);
  usleep(500000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(0x4D00);
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(300000);
  kviktest_send_key(KEY_ENTER); /* Brief */
  usleep(500000);
  kviktest_send_key(KEY_CTRL_P); /* hide left panel */
  usleep(500000);
}

static void run_tests(void) {
  test_help_pgdn_pgup();
  test_help_home_end();
  test_help_shift_tab();
  test_help_deep_traverse();
  test_tree_pgdn_pgup();
  test_tree_enter_select();
  test_hex_search();
  test_hex_tiny_file();
  test_full_mode_navigate();
  test_full_mode_both_panels();
}

TEST_MAIN("test_nav_edges", "coverage_nav_edges.bin")
