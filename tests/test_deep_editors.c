/*
 * test_deep_editors.c: viewer/editor context help and search.
 * Split from test_coverage_deep.c for faster parallel CI.
 */
#include "test_common.h"

/* ---- Context help from viewer ---- */
static void test_viewer_help(void) {
  printf("\n--- Help from viewer context ---\n");

  /* Navigate to HELLO.TXT, F3 to view. */
  navigate_to("hello", "HELLO");
  kviktest_send_key(KEY_F3);
  usleep(1500000);
  check(kviktest_is_running(), "viewer opened");

  /* F1 = Help from viewer context (different help page). */
  kviktest_send_key(KEY_F1);
  usleep(1500000);
  check(kviktest_is_running(), "help from viewer");

  /* Navigate help: PgDn, Tab, Down. */
  kviktest_send_key(0x5100);  /* PgDn */
  usleep(300000);
  kviktest_send_key(KEY_TAB);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);

  /* ESC to close help. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_is_running(), "back to viewer from help");

  /* ESC to close viewer. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels from viewer");
}

/* ---- Context help from editor ---- */
static void test_editor_help(void) {
  printf("\n--- Help from editor context ---\n");

  /* Navigate to HELLO.TXT, F4 to edit. */
  navigate_to("hello", "HELLO");
  kviktest_send_key(KEY_F4);
  usleep(1500000);
  check(kviktest_is_running(), "editor opened");

  /* F1 = Help from editor context. */
  kviktest_send_key(KEY_F1);
  usleep(1500000);
  check(kviktest_is_running(), "help from editor");

  /* Navigate help. */
  kviktest_send_key(0x5100);  /* PgDn */
  usleep(300000);
  kviktest_send_key(0x5100);  /* PgDn */
  usleep(300000);
  kviktest_send_key(0x4900);  /* PgUp */
  usleep(300000);

  /* ESC to close help. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* ESC to close editor (no changes). */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels from editor");
}

/* ---- Dispatch: view file with F3, search text ---- */
static void test_viewer_search(void) {
  printf("\n--- Viewer text search ---\n");

  /* Navigate to README.TXT, F3 to view. */
  navigate_to("readme", "README");
  kviktest_send_key(KEY_F3);
  usleep(1500000);
  check(kviktest_is_running(), "viewer opened for README");

  /* F7 = search in viewer. */
  kviktest_send_key(KEY_F7);
  usleep(500000);

  /* Type search text "test". */
  type_string("test");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "viewer search executed");

  /* Shift+F7 or Ctrl+F7 = search again (repeat). */
  kviktest_send_key(0x5E00);  /* Try Shift+F7 or just send F7 again. */
  usleep(500000);
  kviktest_send_key(KEY_ESC);  /* Dismiss if dialog */
  usleep(500000);

  /* F4 = hex mode toggle. */
  kviktest_send_key(KEY_F4);
  usleep(500000);
  check(kviktest_is_running(), "hex mode in viewer");

  /* F4 back to text. */
  kviktest_send_key(KEY_F4);
  usleep(500000);

  /* ESC to close viewer. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels from viewer search");
}

/* ---- Editor: search and replace ---- */
static void test_editor_search(void) {
  printf("\n--- Editor search ---\n");

  /* Navigate to HELLO.TXT, F4 to edit. */
  navigate_to("hello", "HELLO");
  kviktest_send_key(KEY_F4);
  usleep(1500000);
  check(kviktest_is_running(), "editor opened");

  /* F7 = search in editor. */
  kviktest_send_key(KEY_F7);
  usleep(500000);

  /* Type search text. */
  type_string("Hello");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  /* Dismiss any "not found" or result. */
  if (!kviktest_wait_for_text(23, 0, "C:\\>", 500)) {
    /* Still in editor — good. Search may have found text. */
    check(kviktest_is_running(), "editor search completed");
  }

  /* ESC to close editor. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels from editor");
}

static void run_tests(void) {
  test_viewer_help();
  test_editor_help();
  test_viewer_search();
  test_editor_search();
}

TEST_MAIN("test_deep_editors", "coverage_deep_editors.bin")
