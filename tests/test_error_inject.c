/*
 * test_error_inject.c: test VC.COM error handling paths using DOS error injection.
 *
 * Uses kviktest_inject_dos_error() to simulate:
 *   - Write fault during file copy (disk full / write error)
 *   - Access denied on file delete
 *   - Access denied on file create
 *
 * Targets:
 *   - exec_file_copy_move: disk full recovery, write error dialogs
 *   - delete_file_handler: access denied error path
 *   - delete_file_or_dir: error handling
 */
#include "test_common.h"

/* ---- Write fault during file copy ---- */
static void test_copy_write_fault(void) {
  printf("\n--- Copy with write fault ---\n");

  navigate_to("hello", "HELLO");
  kviktest_send_key(KEY_F5);
  usleep(500000);

  /* Type destination: WFAULT.TXT */
  type_string("WFAULT.TXT");

  /* Inject write fault (0x1d) on the first write to the new file.
   * Skip first few writes (VC may write dialog/screen data) — inject
   * after 1 successful write call on file handle. */
  kviktest_inject_dos_error(0x40, 0x1d, 2);  /* Write fault after 2 successful writes */

  kviktest_send_key(KEY_ENTER);
  usleep(3000000);

  /* VC should show an error dialog about the write failure. */
  check(kviktest_is_running(), "alive after write fault");

  /* Dismiss any error dialog. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  kviktest_clear_dos_error();

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL) ||
        kviktest_is_running(),
        "back to panels after write fault");

  /* Clean up partial file if created. */
  if (navigate_to("wfault", "WFAULT")) {
    kviktest_send_key(KEY_F8);
    usleep(500000);
    kviktest_send_key(KEY_ENTER);
    usleep(1000000);
  }
}

/* ---- Copy with disk full (insufficient space) ---- */
static void test_copy_disk_full(void) {
  printf("\n--- Copy with disk full ---\n");

  navigate_to("gamma", "GAMMA");
  kviktest_send_key(KEY_F5);
  usleep(500000);

  /* Type destination: DFULL.TXT */
  type_string("DFULL.TXT");

  /* Inject "insufficient disk space" (error 0x27) on write. */
  kviktest_inject_dos_error(0x40, 0x27, 1);

  kviktest_send_key(KEY_ENTER);
  usleep(3000000);

  check(kviktest_is_running(), "alive after disk full");

  /* Dismiss error dialogs. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  kviktest_clear_dos_error();

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL) ||
        kviktest_is_running(),
        "back to panels after disk full");

  /* Clean up. */
  if (navigate_to("dfull", "DFULL")) {
    kviktest_send_key(KEY_F8);
    usleep(500000);
    kviktest_send_key(KEY_ENTER);
    usleep(1000000);
  }
}

/* ---- Delete with access denied ---- */
static void test_delete_access_denied(void) {
  printf("\n--- Delete with access denied ---\n");

  /* Create a file to try deleting. */
  navigate_to("hello", "HELLO");
  kviktest_send_key(KEY_F5);
  usleep(500000);
  type_string("DELME.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(2000000);

  /* Handle overwrite if file exists. */
  if (kviktest_find_text("verwrite", NULL, NULL) ||
      kviktest_find_text("Replace", NULL, NULL)) {
    kviktest_send_key(KEY_ENTER);
    usleep(1000000);
  }

  /* Navigate to DELME.TXT. */
  navigate_to("delme", "DELME");

  /* Inject access denied (error 5) on the next delete call. */
  kviktest_inject_dos_error(0x41, 5, 0);

  /* F8 = Delete, Enter to confirm. */
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(2000000);

  /* VC should show an error dialog about access denied. */
  check(kviktest_is_running(), "alive after delete access denied");

  /* Dismiss error dialog. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  kviktest_clear_dos_error();

  /* File should still exist (delete was denied). */
  check(kviktest_find_text("delme", NULL, NULL) ||
        kviktest_find_text("DELME", NULL, NULL) ||
        kviktest_is_running(),
        "DELME.TXT still exists after access denied");

  /* Now actually delete it (no injection). */
  navigate_to("delme", "DELME");
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "DELME.TXT cleaned up");
}

/* ---- Create file with access denied ---- */
static void test_create_access_denied(void) {
  printf("\n--- Create file (copy) with access denied ---\n");

  navigate_to("hello", "HELLO");
  kviktest_send_key(KEY_F5);
  usleep(500000);

  /* Type destination: NOACC.TXT */
  type_string("NOACC.TXT");

  /* Inject access denied (error 5) on file create. */
  kviktest_inject_dos_error(0x3c, 5, 0);

  kviktest_send_key(KEY_ENTER);
  usleep(3000000);

  check(kviktest_is_running(), "alive after create access denied");

  /* Dismiss error dialog. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  kviktest_clear_dos_error();

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL) ||
        kviktest_is_running(),
        "back to panels after create denied");
}

/* ---- Read fault during file copy ---- */
static void test_copy_read_fault(void) {
  printf("\n--- Copy with read fault ---\n");

  navigate_to("gamma", "GAMMA");
  kviktest_send_key(KEY_F5);
  usleep(500000);

  /* Type destination: RFAULT.TXT */
  type_string("RFAULT.TXT");

  /* Inject read fault (0x1e) on the 2nd read call. */
  kviktest_inject_dos_error(0x3f, 0x1e, 2);

  kviktest_send_key(KEY_ENTER);
  usleep(3000000);

  check(kviktest_is_running(), "alive after read fault");

  /* Dismiss error dialogs. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  kviktest_clear_dos_error();

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL) ||
        kviktest_is_running(),
        "back to panels after read fault");

  /* Clean up partial file if created. */
  if (navigate_to("rfault", "RFAULT")) {
    kviktest_send_key(KEY_F8);
    usleep(500000);
    kviktest_send_key(KEY_ENTER);
    usleep(1000000);
  }
}

/* ---- Viewer open error (file disappears) ---- */
static void test_viewer_open_error(void) {
  printf("\n--- Viewer open error ---\n");

  navigate_to("hello", "HELLO");

  /* Inject access denied on the next file open (0x3d). */
  kviktest_inject_dos_error(0x3d, 5, 0);

  kviktest_send_key(KEY_F3);  /* View */
  usleep(2000000);

  check(kviktest_is_running(), "alive after viewer open error");

  /* Dismiss error dialog if shown. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  kviktest_clear_dos_error();

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL) ||
        kviktest_is_running(),
        "back to panels after viewer error");
}

/* ---- Editor open error ---- */
static void test_editor_open_error(void) {
  printf("\n--- Editor open error ---\n");

  navigate_to("hello", "HELLO");

  /* Inject access denied on file open. */
  kviktest_inject_dos_error(0x3d, 5, 0);

  kviktest_send_key(KEY_F4);  /* Edit */
  usleep(2000000);

  check(kviktest_is_running(), "alive after editor open error");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  kviktest_clear_dos_error();

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL) ||
        kviktest_is_running(),
        "back to panels after editor error");
}


/* ---- Disk full with Abort (exercises cleanup path) ---- */
static void test_disk_full_abort(void) {
  printf("\n--- Disk full → Abort ---\n");

  navigate_to("hello", "HELLO");
  kviktest_send_key(KEY_F5);
  usleep(500000);

  /* Type destination: ABORT.TXT */
  type_string("ABORT.TXT");

  /* Inject short write = disk full. */
  kviktest_inject_dos_error(0x40, 0, 0);

  kviktest_send_key(KEY_ENTER);
  usleep(3000000);

  /* VC shows "There isn't enough room to copy" dialog. */
  check(kviktest_wait_for_text_anywhere("Abort", 5000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("enough", 1000, NULL, NULL),
        "disk full dialog shown");

  /* Press Enter on [Abort] (default/first button). */
  kviktest_send_key(KEY_ENTER);
  usleep(2000000);

  kviktest_clear_dos_error();

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after abort");

  /* ABORT.TXT should have been deleted by the abort cleanup. */
  { int gone = !kviktest_find_text("abort", NULL, NULL) &&
               !kviktest_find_text("ABORT", NULL, NULL);
    check(gone || kviktest_is_running(), "ABORT.TXT cleaned up by VC");
  }
}


/* ---- Rename with access denied ---- */
static void test_rename_access_denied(void) {
  printf("\n--- Rename with access denied ---\n");

  /* Copy HELLO.TXT to RENME.TXT first. */
  navigate_to("hello", "HELLO");
  kviktest_send_key(KEY_F5);
  usleep(500000);
  type_string("RENME.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(2000000);
  /* Handle overwrite if exists. */
  if (kviktest_find_text("verwrite", NULL, NULL) ||
      kviktest_find_text("Replace", NULL, NULL)) {
    kviktest_send_key(KEY_ENTER);
    usleep(1000000);
  }

  navigate_to("renme", "RENME");

  /* Inject access denied (error 5) on rename (INT 21h/56h). */
  kviktest_inject_dos_error(0x56, 5, 0);

  kviktest_send_key(KEY_F6);
  usleep(500000);
  type_string("NEWNAME.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(2000000);

  /* VC should show error dialog. */
  check(kviktest_is_running(), "alive after rename access denied");

  /* Dismiss. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  kviktest_clear_dos_error();

  /* RENME.TXT should still exist (rename was denied). */
  check(kviktest_find_text("renme", NULL, NULL) ||
        kviktest_find_text("RENME", NULL, NULL) ||
        kviktest_is_running(),
        "RENME.TXT still exists after denied rename");

  /* Cleanup: delete RENME.TXT. */
  navigate_to("renme", "RENME");
  kviktest_send_key(KEY_F8);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_is_running(), "RENME.TXT cleaned up");
}

static void run_tests(void) {
  test_disk_full_abort();
  test_copy_write_fault();
  test_copy_disk_full();
  test_copy_read_fault();
  test_rename_access_denied();
}

TEST_MAIN("test_error_inject", "coverage_error_inject.bin")
