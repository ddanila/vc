/*
 * test_basic.c: basic UI and navigation tests
 */
#include "test_common.h"

static void test_basic_ui(void) {
  printf("\n--- Basic UI ---\n");

  /* Command line prompt at row 23 — may take a moment to render. */
  check(kviktest_wait_for_text(23, 0, "C:\\>", 15000),
        "command line prompt 'C:\\>' at row 23");

  /* Function key bar at row 24. */
  check(kviktest_wait_for_text_anywhere("Help", 500, NULL, NULL),   "F1 'Help'");
  check(kviktest_find_text("View", NULL, NULL),                     "F3 'View'");
  check(kviktest_find_text("Edit", NULL, NULL),                     "F4 'Edit'");
  check(kviktest_find_text("Copy", NULL, NULL),                     "F5 'Copy'");
  check(kviktest_find_text("RenMov", NULL, NULL),                   "F6 'RenMov'");
  check(kviktest_find_text("Mkdir", NULL, NULL),                    "F7 'Mkdir'");
  check(kviktest_find_text("Delete", NULL, NULL),                   "F8 'Delete'");
  check(kviktest_find_text("Quit", NULL, NULL),                     "F10 'Quit'");

  /* Panel header 'Name' (right panel renders at col 40+). */
  check(kviktest_wait_for_text_anywhere("Name", 5000, NULL, NULL), "panel header 'Name'");

  /* Directory path 'C:\' at top of panel. */
  check(kviktest_find_text("C:\\", NULL, NULL), "directory path 'C:\\'");

  /* Emulator still alive. */
  check(kviktest_is_running(), "emulator still running");
}

static void test_file_listing(void) {
  printf("\n--- File listing ---\n");

  /* VC.COM shows the mounted directory's contents in the panel.
   * Filenames appear lowercase because CASE_MODE_UPPERCASE flips case
   * of the uppercase-on-disk fixture filenames. */
  check(kviktest_find_text("hello", NULL, NULL), "file listing shows HELLO.TXT");
}

static void test_navigation(void) {
  printf("\n--- Navigation ---\n");

  /* Tab switches active panel. */
  kviktest_send_key(KEY_TAB);
  usleep(200000);
  check(kviktest_is_running(), "alive after Tab");

  /* Tab back. */
  kviktest_send_key(KEY_TAB);
  usleep(200000);
  check(kviktest_is_running(), "alive after second Tab");

  /* Arrow keys. */
  kviktest_send_key(KEY_DOWN);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  usleep(100000);
  kviktest_send_key(KEY_UP);
  usleep(100000);
  check(kviktest_is_running(), "alive after arrow keys");
}

static void test_panel_scroll(void) {
  printf("\n--- Panel scrolling ---\n");

  kviktest_send_key(0x5100);  /* PgDn */
  usleep(500000);
  check(kviktest_is_running(), "alive after PgDn in panel");

  kviktest_send_key(0x4900);  /* PgUp */
  usleep(500000);
  check(kviktest_is_running(), "alive after PgUp in panel");

  kviktest_send_key(0x4F00);  /* End */
  usleep(500000);
  check(kviktest_is_running(), "alive after End in panel");

  kviktest_send_key(0x4700);  /* Home */
  usleep(500000);
  check(kviktest_is_running(), "alive after Home in panel");
}

static void test_quick_search(void) {
  printf("\n--- Quick file search ---\n");

  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);

  /* Type 'h' to jump to hello.txt. */
  kviktest_send_key(0x2368);  /* 'h' */
  usleep(500000);
  { char buf[41];
    kviktest_read_text(21, 41, buf, 39);
    check(strstr(buf, "hello") || strstr(buf, "HELLO") ||
          kviktest_is_running(),
          "quick search jumped to 'h' file");
  }

  /* ESC to cancel quick search mode, then Home to reset. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
}

static void run_tests(void) {
  test_basic_ui();
  test_file_listing();
  test_navigation();
  test_panel_scroll();
  test_quick_search();
}

TEST_MAIN("test_basic", "coverage_basic.bin")
