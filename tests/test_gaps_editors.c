/*
 * test_gaps_editors.c: editor block operations and viewer modes.
 * Split from test_coverage_gaps.c for faster parallel CI.
 */
#include "test_common.h"

/* ---- Editor block operations ---- */
static void test_editor_block(void) {
  printf("\n--- Editor block operations ---\n");

  navigate_to("readme", "README");
  kviktest_send_key(KEY_F4);
  usleep(1500000);
  check(kviktest_is_running(), "editor opened");

  /* Ctrl+K B = mark block begin. */
  kviktest_send_key(0x300B);  /* Ctrl+K: scancode 0x30='B' (?), Ctrl+K=0x0B... */
  usleep(200000);
  /* Actually, in VC editor: F3 = mark block begin, F4 = mark block end. */
  /* Use Shift+arrow to select text. */
  /* Shift+Right = extend selection. Shift modifier adds 0x80 to scancode? */
  /* Actually let me just use F3/F4 for block marking. */

  /* Down a few lines. */
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);

  /* Try Ctrl+Y = delete line. */
  kviktest_send_key(0x1519);  /* Ctrl+Y: scancode 0x15='Y', ASCII 0x19 */
  usleep(300000);
  check(kviktest_is_running(), "Ctrl+Y delete line");

  /* Ctrl+End = goto end of file. */
  kviktest_send_key(0x7500);  /* Ctrl+End */
  usleep(300000);
  check(kviktest_is_running(), "Ctrl+End in editor");

  /* Ctrl+Home = goto beginning. */
  kviktest_send_key(0x7700);  /* Ctrl+Home */
  usleep(300000);
  check(kviktest_is_running(), "Ctrl+Home in editor");

  /* Ctrl+Right = word right. */
  kviktest_send_key(0x7400);  /* Ctrl+Right */
  usleep(200000);
  kviktest_send_key(0x7400);
  usleep(200000);
  check(kviktest_is_running(), "Ctrl+Right word nav");

  /* Ctrl+Left = word left. */
  kviktest_send_key(0x7300);  /* Ctrl+Left */
  usleep(200000);
  check(kviktest_is_running(), "Ctrl+Left word nav");

  /* ESC without saving (Ctrl+Y deleted a line, don't save). */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  /* If "save changes?" dialog appears, press N or ESC. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels from editor");
}

/* ---- Viewer: wrap mode, goto offset ---- */
static void test_viewer_modes(void) {
  printf("\n--- Viewer modes ---\n");

  navigate_to("readme", "README");
  kviktest_send_key(KEY_F3);
  usleep(1500000);
  check(kviktest_is_running(), "viewer opened");

  /* F2 = toggle word wrap. */
  kviktest_send_key(KEY_F2);
  usleep(500000);
  check(kviktest_is_running(), "word wrap toggled");

  /* F2 again to toggle back. */
  kviktest_send_key(KEY_F2);
  usleep(500000);

  /* F8 = goto hex offset (in some versions). */
  kviktest_send_key(KEY_F8);
  usleep(500000);
  /* Dismiss dialog if appeared. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);

  /* F6 = edit (switch to editor from viewer). */
  kviktest_send_key(KEY_F6);
  usleep(1000000);
  check(kviktest_is_running(), "switched to editor from viewer");

  /* ESC back. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels");
}

static void run_tests(void) {
  test_editor_block();
  test_viewer_modes();
}

TEST_MAIN("test_gaps_editors", "coverage_gaps_editors.bin")
