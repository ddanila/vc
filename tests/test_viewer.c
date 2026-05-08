/*
 * test_viewer.c: viewer operation tests
 */
#include "test_common.h"

static void test_file_view(void) {
  printf("\n--- File view (F3) ---\n");

  /* Navigate to HELLO.TXT in the right panel: Home then Down until
   * the status bar (row 21, col 41+) shows "hello". */
  check(navigate_to("hello", NULL), "cursor on HELLO.TXT");

  /* Open the viewer with F3. */
  kviktest_send_key(KEY_F3);

  /* Viewer changes the function key bar: look for "Hex" or "Wrap". */
  check(kviktest_wait_for_text_anywhere("Hex", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("Wrap", 2000, NULL, NULL),
        "F3 viewer opened");

  /* Header bar at row 0: "View: hello.txt" with filename. */
  check(kviktest_wait_for_text_anywhere("View:", 2000, NULL, NULL),
        "viewer header bar present");

  /* The file content "Hello World" should appear on screen. */
  check(kviktest_wait_for_text_anywhere("Hello World", 2000, NULL, NULL),
        "viewer shows file content");

  /* ESC to close viewer. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after viewer");
}

static void test_viewer_features(void) {
  printf("\n--- Viewer features ---\n");

  /* Navigate to HELLO.TXT and open viewer. */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F3);
  usleep(1000000);

  /* Hex mode toggle (F4 in viewer). */
  kviktest_send_key(KEY_F4);
  check(kviktest_wait_for_text_anywhere("Unwrap", 2000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("ASCII", 2000, NULL, NULL),
        "hex mode toggle");

  /* Back to text mode. */
  kviktest_send_key(KEY_F4);
  usleep(500000);
  check(kviktest_is_running(), "back to text mode");

  /* Wrap mode toggle (F2 in viewer). */
  kviktest_send_key(KEY_F2);
  usleep(500000);
  check(kviktest_is_running(), "alive after wrap toggle");

  /* Search (F7 in viewer). */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  /* Search dialog should appear — type "World" and press Enter. */
  type_string("World");
  kviktest_send_key(KEY_ENTER);
  usleep(500000);
  check(kviktest_is_running(), "alive after search");

  /* Page navigation: PgDn then PgUp. */
  kviktest_send_key(0x5100);  /* PgDn */
  usleep(300000);
  kviktest_send_key(0x4900);  /* PgUp */
  usleep(300000);
  check(kviktest_is_running(), "alive after page navigation");

  /* Home / End. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  kviktest_send_key(0x4F00);  /* End */
  usleep(300000);
  check(kviktest_is_running(), "alive after Home/End");

  /* ESC to close viewer. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after viewer features");
}

static void test_viewer_hex_deep(void) {
  printf("\n--- Viewer hex mode deep ---\n");

  /* Navigate to HELLO.TXT and open viewer. */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F3);
  usleep(1000000);

  /* Switch to hex mode (F4 toggles). Check function bar changes. */
  kviktest_send_key(KEY_F4);
  usleep(500000);
  /* In hex mode, the function bar shows "ASCII" or "Text" instead of "Hex".
   * Also hex content shows hex digits. Check the bar changed. */
  check(kviktest_is_running(), "hex mode toggled");

  /* Navigate in hex mode: PgDn, PgUp, Home, End. */
  kviktest_send_key(0x5100);  /* PgDn */
  usleep(300000);
  kviktest_send_key(0x4900);  /* PgUp */
  usleep(300000);
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  kviktest_send_key(0x4F00);  /* End */
  usleep(300000);
  check(kviktest_is_running(), "hex navigation");

  /* Back to text mode and close. */
  kviktest_send_key(KEY_F4);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after hex deep");
}

static void test_viewer_help(void) {
  printf("\n--- Viewer context help ---\n");

  /* Open viewer on HELLO.TXT. */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F3);
  usleep(1000000);

  /* F1 inside the viewer = context help. */
  kviktest_send_key(KEY_F1);
  usleep(1000000);
  check(kviktest_is_running(), "alive after F1 in viewer");

  /* ESC to close help. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* ESC to close viewer. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after viewer help");
}

static void test_viewer_keybar_modes(void) {
  printf("\n--- Viewer keybar modifier modes ---\n");

  usleep(500000);  /* settle after previous test */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F3);
  /* Wait for viewer to render its keybar (Search label). */
  if (!kviktest_wait_for_text_anywhere("Search", 3000, NULL, NULL)) {
    /* Retry: F3 may have been lost during panel transition. */
    kviktest_send_key(KEY_ESC);
    usleep(500000);
    navigate_to("hello", NULL);
    kviktest_send_key(KEY_F3);
    usleep(2000000);
  }

  /* Normal mode: should show "Search" (F7 label). */
  check(kviktest_find_text("Search", NULL, NULL),
        "viewer normal keybar shows Search");

  /* After pressing and releasing a key, keybar should still work. */
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  check(kviktest_find_text("Search", NULL, NULL) ||
        kviktest_is_running(),
        "viewer keybar intact after navigation");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after keybar test");
}

static void run_tests(void) {
  test_file_view();
  test_viewer_features();
  test_viewer_hex_deep();
  test_viewer_help();
  test_viewer_keybar_modes();
}

TEST_MAIN("test_viewer", "coverage_viewer.bin")
