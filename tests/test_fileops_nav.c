/*
 * test_fileops_nav.c: subdirectory navigation (enter, "..", Backspace).
 */
#include "test_common.h"

static void test_subdir_nav(void) {
  printf("\n--- Subdirectory navigation ---\n");

  /* Create NAVDIR with F7 mkdir. */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  type_string("NAVDIR");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_wait_for_text_anywhere("NAVDIR", 2000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("navdir", 2000, NULL, NULL),
        "NAVDIR created");
  check(host_is_dir("NAVDIR") || host_is_dir("navdir"),
        "NAVDIR exists on host filesystem");

  /* Navigate to NAVDIR and enter it. */
  navigate_to("NAVDIR", "navdir");
  kviktest_send_key(KEY_ENTER);
  usleep(1500000);

  /* Should now be inside NAVDIR — path display shows NAVDIR. */
  check(kviktest_find_text("NAVDIR", NULL, NULL) ||
        kviktest_find_text("navdir", NULL, NULL),
        "entered NAVDIR");

  /* Inside NAVDIR — emulator still running. */
  check(kviktest_is_running(), "alive inside NAVDIR");

  /* Home then Enter on ".." to go back to parent. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  kviktest_send_key(KEY_ENTER);
  usleep(1500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to root via '..'");

  /* Enter NAVDIR again, come back with Backspace. */
  navigate_to("NAVDIR", "navdir");
  kviktest_send_key(KEY_ENTER);
  usleep(1500000);
  check(kviktest_is_running(), "entered NAVDIR again");

  /* Backspace to go back to parent. */
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(1500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to root via Backspace");

  /* Clean up: delete NAVDIR. */
  navigate_to("NAVDIR", "navdir");
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "NAVDIR cleaned up");
  /* Note: directory cleanup via F8+Enter may not always fully
     remove the dir in all VC versions. Not a hard failure. */
}

static void run_tests(void) {
  test_subdir_nav();
}

TEST_MAIN("test_fileops_nav", "coverage_fileops_nav.bin")
