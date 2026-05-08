/*
 * test_panel_keys.c: keyboard combos and file list dispatch keys.
 * Split from test_panel_adv.c for faster parallel CI.
 */
#include "test_common.h"

/* ---- Event loop: additional key combos ---- */
static void test_extra_keys(void) {
  printf("\n--- Extra key combos ---\n");

  /* Ctrl+R = re-read current directory. */
  kviktest_send_key(0x1312);  /* Ctrl+R: scancode 0x13='R', ASCII 0x12 */
  usleep(1000000);
  check(kviktest_is_running(), "alive after Ctrl+R (re-read)");

  /* Ctrl+L = drive info / repaint. */
  kviktest_send_key(0x260C);  /* Ctrl+L: scancode 0x26='L', ASCII 0x0C */
  usleep(500000);
  /* If a dialog appeared, dismiss it. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+L");

  /* Alt+F5 = unpack (may not apply, just test the key). */
  kviktest_send_key(0x6C00);  /* Alt+F5 */
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_is_running(), "alive after Alt+F5");

  /* Alt+F6 = pack (may not apply). */
  kviktest_send_key(0x6D00);  /* Alt+F6 */
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_is_running(), "alive after Alt+F6");

  /* Alt+F7 = find file. */
  kviktest_send_key(0x6E00);  /* Alt+F7 */
  usleep(500000);
  /* If find dialog appeared, close it. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_is_running(), "alive after Alt+F7");

  /* Ctrl+F2 = toggle right panel on/off. */
  kviktest_send_key(0x5F00);  /* Ctrl+F2 */
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+F2 (toggle right panel)");
  /* Restore. */
  kviktest_send_key(0x5F00);  /* Ctrl+F2 */
  usleep(500000);

  /* Ctrl+F6 = sort by size. */
  kviktest_send_key(0x6300);  /* Ctrl+F6 */
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+F6 (sort by size)");

  /* Ctrl+F7 = unsorted. */
  kviktest_send_key(0x6400);  /* Ctrl+F7 */
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+F7 (unsorted)");
}

/* ---- File list dispatch: additional keys ---- */
static void test_file_list_keys(void) {
  printf("\n--- File list dispatch keys ---\n");

  /* Ctrl+PgDn = enter directory / view archive. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  navigate_to("subdir2", "SUBDIR2");
  kviktest_send_key(0x7600);  /* Ctrl+PgDn */
  usleep(1500000);
  check(kviktest_is_running(), "Ctrl+PgDn entered subdir");

  /* Ctrl+PgUp = go to parent. */
  kviktest_send_key(0x8400);  /* Ctrl+PgUp */
  usleep(1500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "Ctrl+PgUp back to root");

  /* Ctrl+Home = go to top of file list. */
  kviktest_send_key(0x7700);  /* Ctrl+Home */
  usleep(300000);
  check(kviktest_is_running(), "Ctrl+Home");

  /* Ctrl+End = go to bottom of file list. */
  kviktest_send_key(0x7500);  /* Ctrl+End */
  usleep(300000);
  check(kviktest_is_running(), "Ctrl+End");

  /* Left arrow in file list (switch columns in brief mode). */
  kviktest_send_key(KEY_LEFT);
  usleep(200000);
  kviktest_send_key(KEY_RIGHT);
  usleep(200000);
  check(kviktest_is_running(), "Left/Right in file list");
}

static void run_tests(void) {
  test_extra_keys();
  test_file_list_keys();
}

TEST_MAIN("test_panel_keys", "coverage_panel_keys.bin")
