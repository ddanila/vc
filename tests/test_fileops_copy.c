/*
 * test_fileops_copy.c: file copy, rename, and basic mkdir/delete operations.
 */
#include "test_common.h"

static void test_file_ops(void) {
  printf("\n--- File operations (F7/F8) ---\n");

  /* F7 = Mkdir: create a test directory. */
  kviktest_send_key(KEY_F7);
  check(kviktest_wait_for_text_anywhere("Create the directory", 2000, NULL, NULL),
        "F7 mkdir dialog opened");

  /* Type directory name "TESTDIR" and press Enter. */
  type_string("TESTDIR");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "alive after F7 mkdir");

  /* Verify TESTDIR appears in the panel. */
  check(kviktest_wait_for_text_anywhere("TESTDIR", 2000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("testdir", 2000, NULL, NULL),
        "TESTDIR visible in panel");
  check(host_is_dir("TESTDIR") || host_is_dir("testdir"),
        "TESTDIR exists on host filesystem");

  /* Navigate to TESTDIR and delete it with F8. */
  navigate_to("TESTDIR", "testdir");

  kviktest_send_key(KEY_F8);
  usleep(500000);

  /* Confirm deletion (Enter on the confirm dialog). */
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "alive after F8 delete");

  /* Verify TESTDIR is gone. */
  { int gone = !kviktest_find_text("TESTDIR", NULL, NULL) &&
               !kviktest_find_text("testdir", NULL, NULL);
    check(gone || kviktest_is_running(), "TESTDIR deleted or still alive");
  }
  check(!host_path_exists("TESTDIR") && !host_path_exists("testdir"),
        "TESTDIR gone from host filesystem");
}

static void test_file_copy(void) {
  printf("\n--- File copy (F5) ---\n");

  /* Navigate to HELLO.TXT. */
  navigate_to("hello", NULL);

  /* F5 = Copy. */
  kviktest_send_key(KEY_F5);
  usleep(500000);

  /* Copy dialog opens with the destination input pre-populated with the
   * source filename. 4.05 starts the field with the text selected so a
   * type replaces it; 4.99.09 leaves the cursor at the end with no
   * selection, so the same type would append. Use Ctrl+Y (delete entire
   * line) which behaves identically across both builds. */
  kviktest_send_key(0x1519);  /* Ctrl+Y */
  usleep(200000);

  /* Type the destination: COPY.TXT */
  type_string("COPY.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(1500000);
  check(kviktest_is_running(), "alive after F5 copy");

  /* Verify COPY.TXT appears in the panel. */
  check(kviktest_wait_for_text_anywhere("copy", 2000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("COPY", 2000, NULL, NULL),
        "COPY.TXT visible in panel");
  check(host_path_exists("COPY.TXT") || host_path_exists("copy.txt"),
        "COPY.TXT exists on host filesystem");
  check(host_files_match("HELLO.TXT", "COPY.TXT") ||
        host_files_match("HELLO.TXT", "copy.txt"),
        "COPY.TXT content matches HELLO.TXT");

  /* Clean up: navigate to COPY.TXT and delete with F8. */
  navigate_to("copy", "COPY");
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);  /* Confirm delete */
  usleep(1000000);
  check(kviktest_is_running(), "cleanup: COPY.TXT deleted");
  check(!host_path_exists("COPY.TXT") && !host_path_exists("copy.txt"),
        "COPY.TXT gone from host filesystem");
}

static void test_file_rename(void) {
  printf("\n--- File rename (F6) ---\n");

  /* Navigate to DATA.BIN. */
  navigate_to("data", "DATA");

  /* F6 = RenMov. */
  kviktest_send_key(KEY_F6);
  usleep(500000);

  /* Type new name: TEMP.BIN */
  type_string("TEMP.BIN");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "alive after F6 rename");

  /* Verify TEMP.BIN appears (may be lowercase due to case flip). */
  check(kviktest_wait_for_text_anywhere("temp", 2000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("TEMP", 2000, NULL, NULL) ||
        kviktest_is_running(),
        "alive after rename to TEMP.BIN");
  check(host_path_exists("TEMP.BIN") || host_path_exists("temp.bin"),
        "TEMP.BIN exists on host filesystem after rename");
  check(!host_path_exists("DATA.BIN") && !host_path_exists("data.bin"),
        "DATA.BIN gone from host filesystem after rename");

  /* Rename back: navigate to TEMP.BIN, F6 -> DATA.BIN. */
  navigate_to("temp", "TEMP");
  kviktest_send_key(KEY_F6);
  usleep(500000);
  type_string("DATA.BIN");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "alive after rename back");
  check(host_path_exists("DATA.BIN") || host_path_exists("data.bin"),
        "DATA.BIN restored on host filesystem");
}

static void run_tests(void) {
  test_file_ops();
  test_file_copy();
  test_file_rename();
}

TEST_MAIN("test_fileops_copy", "coverage_fileops_copy.bin")
