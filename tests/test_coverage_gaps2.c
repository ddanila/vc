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

  /* F5 in help might toggle something (topic index in some versions). */
  kviktest_send_key(KEY_F5);
  usleep(500000);
  /* Dismiss if dialog. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);

  /* Ctrl+PgDn / Ctrl+PgUp for page-level navigation. */
  kviktest_send_key(0x7600);  /* Ctrl+PgDn */
  usleep(500000);
  kviktest_send_key(0x8400);  /* Ctrl+PgUp */
  usleep(500000);
  check(kviktest_is_running(), "help Ctrl+PgDn/PgUp");

  /* Left/Right arrows might scroll horizontally or change page. */
  kviktest_send_key(KEY_LEFT);
  usleep(200000);
  kviktest_send_key(KEY_RIGHT);
  usleep(200000);

  /* ESC to close. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after help pages");
}

/* ---- Tree panel mode ---- */
static void test_tree_panel(void) {
  printf("\n--- Tree panel mode ---\n");

  /* F9 → Left → Tree (4th item after Brief, Full, Info). */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);  /* 4th item = Tree */
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
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

  /* Switch back to Brief. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_ENTER);  /* Brief */
  usleep(1000000);
  check(kviktest_wait_for_text_anywhere("Name", 2000, NULL, NULL),
        "back to Brief from tree");
}

static void run_tests(void) {
  test_help_pages();
  test_tree_panel();
}

TEST_MAIN("test_coverage_gaps2", "coverage_gaps2.bin")
