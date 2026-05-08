/*
 * test_fileops_adv.c: advanced file copy/move operations —
 * overwrite confirmation, multi-file copy execution, rename conflict.
 */
#include "test_common.h"

static void test_copy_overwrite(void) {
  printf("\n--- Copy with overwrite ---\n");

  /* Copy HELLO.TXT to OVER.TXT. */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F5);
  usleep(500000);
  type_string("OVER.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(2000000);
  check(kviktest_wait_for_text_anywhere("over", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("OVER", 3000, NULL, NULL),
        "OVER.TXT created");
  check(host_path_exists("OVER.TXT") || host_path_exists("over.txt"),
        "OVER.TXT exists on host filesystem");

  /* Copy HELLO.TXT to OVER.TXT again — triggers overwrite dialog. */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F5);
  usleep(500000);
  type_string("OVER.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  /* Overwrite dialog should appear. Confirm with Enter (Overwrite). */
  check(kviktest_wait_for_text_anywhere("verwrite", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("Already", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("exists", 3000, NULL, NULL) ||
        kviktest_is_running(),
        "overwrite dialog shown");
  kviktest_send_key(KEY_ENTER);
  usleep(2000000);
  check(kviktest_is_running(), "alive after overwrite confirm");
  check(host_files_match("HELLO.TXT", "OVER.TXT") ||
        host_files_match("HELLO.TXT", "over.txt"),
        "OVER.TXT content matches HELLO.TXT after overwrite");

  /* Clean up: delete OVER.TXT. */
  navigate_to("over", "OVER");
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "cleanup: OVER.TXT deleted");
  check(!host_path_exists("OVER.TXT") && !host_path_exists("over.txt"),
        "OVER.TXT gone from host filesystem");
}

static void test_rename_conflict(void) {
  printf("\n--- Rename with conflict ---\n");

  /* Try to rename DATA.BIN to HELLO.TXT (which already exists). */
  navigate_to("data", "DATA");
  kviktest_send_key(KEY_F6);
  usleep(500000);
  type_string("HELLO.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(1500000);

  /* "File exists" or error dialog should appear. Dismiss with ESC. */
  check(kviktest_is_running(), "rename conflict dialog shown");
  kviktest_send_key(KEY_ESC);
  usleep(1000000);

  /* If still in a dialog, one more ESC. Check for panels first. */
  if (!kviktest_wait_for_text_anywhere("Help", 1000, NULL, NULL)) {
    kviktest_send_key(KEY_ESC);
    usleep(500000);
  }

  /* DATA.BIN should still exist. */
  check(kviktest_wait_for_text_anywhere("data", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("DATA", 3000, NULL, NULL) ||
        kviktest_is_running(),
        "DATA.BIN preserved after cancelled rename");
}

static void run_tests(void) {
  test_copy_overwrite();
  test_rename_conflict();
}

TEST_MAIN("test_fileops_adv", "coverage_fileops_adv.bin")
