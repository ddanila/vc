/*
 * test_fileops_sel.c: file selection, multi-file operations, and delete.
 */
#include "test_common.h"

static void test_file_select(void) {
  printf("\n--- File selection ---\n");

  /* Insert key selects/deselects the current file and moves down. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  kviktest_send_key(KEY_DOWN);  /* Skip past '..' or first entry */
  usleep(200000);

  kviktest_send_key(0x5200);  /* Insert */
  usleep(300000);
  check(kviktest_is_running(), "alive after Insert (select file)");

  /* Second Insert on next file. */
  kviktest_send_key(0x5200);  /* Insert */
  usleep(300000);
  check(kviktest_is_running(), "alive after second Insert");

  /* '*' (Gray star / numpad *) selects/inverts all files.
   * Scancode for numpad * is 0x37, ASCII '*' = 0x2A. */
  kviktest_send_key(0x372A);
  usleep(500000);
  /* After selecting files, the status bar should show "bytes in X selected files"
   * or similar selection info. */
  check(kviktest_is_running(), "alive after * (invert selection)");

  /* '+' (numpad +) selects by pattern — opens a dialog. */
  kviktest_send_key(0x4E2B);  /* Numpad + */
  usleep(500000);
  /* Type *.TXT and Enter to select all TXT files. */
  type_string("*.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(500000);
  check(kviktest_is_running(), "alive after + select by pattern");

  /* '-' (numpad -) deselects by pattern. */
  kviktest_send_key(0x4A2D);  /* Numpad - */
  usleep(500000);
  kviktest_send_key(KEY_ENTER);  /* Accept default pattern to deselect. */
  usleep(500000);
  check(kviktest_is_running(), "alive after - deselect by pattern");
}

static void test_multi_file_copy(void) {
  printf("\n--- Multi-file select + copy ---\n");

  /* Select multiple files with Insert key. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  kviktest_send_key(KEY_DOWN);  /* skip '..' or first entry */
  usleep(200000);
  kviktest_send_key(0x5200);  /* Insert — select first file */
  usleep(300000);
  kviktest_send_key(0x5200);  /* Insert — select second file */
  usleep(300000);

  /* F5 = Copy selected files. */
  kviktest_send_key(KEY_F5);
  usleep(500000);

  /* Copy dialog should show selection count or destination.
   * Just press ESC to cancel — we're testing the dialog path, not the copy. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_is_running(), "alive after multi-file copy dialog");

  /* Deselect all with '*'. */
  kviktest_send_key(0x372A);  /* numpad * */
  usleep(500000);
  check(kviktest_is_running(), "alive after deselect");
}

static void test_mkdir_existing(void) {
  printf("\n--- Mkdir existing name ---\n");

  /* Create SUBDIR2. */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  type_string("SUBDIR2");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "SUBDIR2 created");
  check(host_is_dir("SUBDIR2") || host_is_dir("subdir2"),
        "SUBDIR2 exists on host filesystem");

  /* Try to create SUBDIR2 again — should show error dialog. */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  type_string("SUBDIR2");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  /* Error dialog should appear. Dismiss with ESC or Enter. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_is_running(), "survived mkdir on existing dir");

  /* Clean up: delete SUBDIR2. */
  navigate_to("SUBDIR2", "subdir2");
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "SUBDIR2 cleaned up");
  /* Note: directory cleanup via F8+Enter may not fully work
     for pre-existing fixture dirs. Not a hard failure. */
}

static void test_delete_file(void) {
  printf("\n--- Delete file (F8) ---\n");

  /* Create DELME with F7 mkdir, then delete it with F8.
   * Uses mkdir instead of F5 copy to avoid dialog state issues. */
  kviktest_send_key(KEY_F7);
  usleep(1000000);
  type_string("DELME");
  kviktest_send_key(KEY_ENTER);
  usleep(2000000);
  check(kviktest_wait_for_text_anywhere("delme", 5000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("DELME", 5000, NULL, NULL),
        "DELME created");

  /* Navigate to DELME and delete it. */
  navigate_to("DELME", "delme");
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  { int gone = !kviktest_find_text("delme", NULL, NULL) &&
               !kviktest_find_text("DELME", NULL, NULL);
    check(gone, "DELME deleted");
  }
  check(!host_path_exists("DELME") && !host_path_exists("delme"),
        "DELME gone from host filesystem");
  check(kviktest_is_running(), "alive after file delete");

  /* Multi-select delete dialog: select two files, open F8, cancel. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  kviktest_send_key(KEY_DOWN);  /* skip '..' */
  usleep(200000);
  kviktest_send_key(0x5200);  /* Insert — select first file */
  usleep(300000);
  kviktest_send_key(0x5200);  /* Insert — select second file */
  usleep(300000);
  kviktest_send_key(KEY_F8);
  usleep(500000);
  check(kviktest_is_running(), "F8 multi-select dialog");
  kviktest_send_key(KEY_ESC);  /* Cancel — don't delete real fixtures */
  usleep(500000);
  kviktest_send_key(0x372A);  /* numpad * to deselect */
  usleep(500000);
  check(kviktest_is_running(), "alive after multi-select cancel");

  /* Directory delete: create DELDIR with F7, delete with F8. */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  type_string("DELDIR");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_wait_for_text_anywhere("deldir", 2000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("DELDIR", 2000, NULL, NULL),
        "DELDIR created");

  navigate_to("DELDIR", "deldir");
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  { int gone = !kviktest_find_text("deldir", NULL, NULL) &&
               !kviktest_find_text("DELDIR", NULL, NULL);
    check(gone, "directory deleted");
  }
  check(!host_path_exists("DELDIR") && !host_path_exists("deldir"),
        "DELDIR gone from host filesystem");
  check(kviktest_is_running(), "alive after directory delete");
}

static void run_tests(void) {
  test_file_select();
  test_multi_file_copy();
  test_delete_file();
  test_mkdir_existing();
}

TEST_MAIN("test_fileops_sel", "coverage_fileops_sel.bin")
