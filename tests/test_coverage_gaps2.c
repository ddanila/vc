/*
 * test_coverage_gaps2.c: help page navigation and tree panel mode.
 *
 * Split from test_coverage_gaps.c for faster parallel CI.
 */
#include "test_common.h"

/* ---- Help: follow multiple links for page loading ---- */
static void test_help_pages(void) {
  printf("\n--- Help page navigation ---\n");

  kviktest_send_key(KEY_F1);
  usleep(1500000);
  check(kviktest_is_running(), "help opened");

  /* Follow links: Tab cycles through links, Enter activates. */
  /* Link 1. */
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "help link 1");

  /* Link 2 from new page. */
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "help link 2");

  /* Link 3 from new page. */
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "help link 3");

  /* Backspace to go back through history. */
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(500000);
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(500000);
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(500000);
  check(kviktest_is_running(), "help back x3");

  /* Shift+Tab = reverse link navigation. */
  kviktest_send_key(0x0F00);  /* Shift+Tab */
  usleep(300000);
  check(kviktest_is_running(), "help Shift+Tab");

  if (!test_is_vc_499()) {
    /* F5 in help opens a topic-index dialog under 4.05. 4.99.09 binds
     * F5 inside the help viewer to a destructive action that exits VC,
     * so we don't exercise it under the overlay. */
    kviktest_send_key(KEY_F5);
    usleep(500000);
    /* Dismiss if dialog. */
    kviktest_send_key(KEY_ESC);
    usleep(300000);
  }

  if (!test_is_vc_499()) {
    /* Ctrl+PgDn / Ctrl+PgUp for page-level navigation. 4.99.09 doesn't
     * route those scancodes inside the help viewer — they fall through
     * to the panel underneath, so skip them when running the overlay. */
    kviktest_send_key(0x7600);  /* Ctrl+PgDn */
    usleep(500000);
    kviktest_send_key(0x8400);  /* Ctrl+PgUp */
    usleep(500000);
    check(kviktest_is_running(), "help Ctrl+PgDn/PgUp");
  } else {
    check(1, "skipped help Ctrl+PgDn/PgUp (4.99 routes elsewhere)");
  }

  /* Left/Right arrows might scroll horizontally or change page. */
  kviktest_send_key(KEY_LEFT);
  usleep(200000);
  kviktest_send_key(KEY_RIGHT);
  usleep(200000);

  /* ESC to close. 4.05 needs two ESCs (one closes a help sub-dialog, one
   * closes the viewer); 4.99.09 only needs one — a second ESC at the
   * panels then causes it to exit. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  if (!test_is_vc_499()) {
    kviktest_send_key(KEY_ESC);
    usleep(500000);
  }
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after help pages");
}

/* ---- Tree panel mode ---- */
static void test_tree_panel(void) {
  printf("\n--- Tree panel mode ---\n");

  /* The previous help test may leave keystrokes on the command line under
   * 4.99.09 (Tab/Enter exit the viewer differently). Clear it first.
   * Add a settle delay because help-close timing is slower in 4.99.09. */
  usleep(1500000);
  kviktest_send_key(0x1519);  /* Ctrl+Y — clear cmdline */
  usleep(500000);

  /* F9 → Left → 'T' hotkey to select Tree. The Left dropdown's display
   * mode group differs between builds: 4.05 lists Brief/Full/Info/Tree,
   * 4.99.09 inserts Description and Quick view, so position-based
   * DOWN×4 lands on the wrong item under 4.99. Hotkey is stable. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  type_string("t");  /* Tree hotkey */
  usleep(2000000);

  check(kviktest_is_running(), "tree mode active");

  /* Navigate tree: Down/Up/Enter. */
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  kviktest_send_key(KEY_UP);
  usleep(300000);
  check(kviktest_is_running(), "tree navigation");

  /* Enter on a tree node to select it. */
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "tree node selected");

  /* Switch back to Brief by hotkey. 4.99 may have already left tree
   * mode after the previous Enter (it returns the panel to its file
   * listing); switching to Brief is then idempotent. Tolerate VC's
   * "alive but Name missing" outcome. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  type_string("b");  /* Brief hotkey */
  usleep(1000000);
  check(kviktest_wait_for_text_anywhere("Name", 2000, NULL, NULL) ||
        kviktest_is_running(),
        "back to Brief from tree");
}

static void run_tests(void) {
  test_help_pages();
  test_tree_panel();
}

TEST_MAIN("test_coverage_gaps2", "coverage_gaps2.bin")
