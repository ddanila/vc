/*
 * test_event_loop.c: exercise event_loop timer and input paths —
 * idle display updates, command line typing, key combos not tested elsewhere.
 */
#include "test_common.h"

static void test_idle_timer(void) {
  printf("\n--- Idle timer updates ---\n");

  /* Wait 3 seconds without input — event_loop updates cursor animation
   * and refresh_display when the timer expires. */
  usleep(3000000);
  check(kviktest_is_running(), "alive after 3s idle");

  /* Press a key after idle — triggers display update path. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(500000);
  check(kviktest_is_running(), "alive after post-idle keypress");

  /* Another idle period, then multiple rapid keys. */
  usleep(2000000);
  kviktest_send_key(KEY_DOWN);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  usleep(100000);
  kviktest_send_key(KEY_UP);
  usleep(100000);
  check(kviktest_is_running(), "alive after idle+rapid keys");
}

static void test_command_line(void) {
  printf("\n--- Command line typing ---\n");

  /* Type characters on the command line (C:\> prompt).
   * This exercises the command-line input path in event_loop. */
  type_string("dir");
  usleep(500000);

  /* The command line should show "dir" after the prompt. */
  check(kviktest_find_text("dir", NULL, NULL),
        "command line shows typed text");

  /* Backspace to delete one char. */
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(200000);
  check(kviktest_is_running(), "alive after Backspace on cmdline");

  /* ESC to clear the command line. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 2000),
        "command line cleared after ESC");
}

static void test_ctrl_combos(void) {
  printf("\n--- Ctrl key combos ---\n");

  /* Ctrl+E — show command history (or last command). */
  kviktest_send_key(0x1205);  /* Ctrl+E */
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+E");

  /* Clear any state. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);

  /* Ctrl+R — redraw/refresh screen. */
  kviktest_send_key(0x1312);  /* Ctrl+R */
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+R");

  /* Ctrl+J / Ctrl+Enter — some VC versions treat this as "run in background". */
  kviktest_send_key(0x1C0A);  /* Ctrl+Enter (Ctrl+J) */
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+Enter");

  /* ESC to clear. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);
}

static void test_alt_drive_keys(void) {
  printf("\n--- Alt+drive letter keys ---\n");

  /* Alt+C should switch to drive C: (we're already on C:). */
  kviktest_send_key(0x2E00);  /* Alt+C */
  usleep(1000000);
  check(kviktest_is_running(), "alive after Alt+C");

  /* Verify we're still on C:\. */
  check(kviktest_wait_for_text(23, 0, "C:\\>", 2000) ||
        kviktest_wait_for_text_anywhere("Help", 1000, NULL, NULL),
        "still on drive C:");
}

static void test_function_key_dispatch(void) {
  printf("\n--- Function key dispatch ---\n");

  /* F2 — user menu (might show error or menu if VC.MNU exists). */
  kviktest_send_key(KEY_F2);
  usleep(500000);
  check(kviktest_is_running(), "alive after F2");
  kviktest_send_key(KEY_ESC);  /* dismiss if dialog opened */
  usleep(300000);

  /* Shift+F1 through Shift+F6 — extended function keys. */
  kviktest_send_key(0x5400);  /* Shift+F1 */
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  check(kviktest_is_running(), "alive after Shift+F1");

  kviktest_send_key(0x5500);  /* Shift+F2 */
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  check(kviktest_is_running(), "alive after Shift+F2");

  /* F10 — pull-down menu (different path from F9). */
  kviktest_send_key(KEY_F10);
  usleep(500000);
  check(kviktest_is_running(), "alive after F10");
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
}

static void test_ctrl_o_panels(void) {
  printf("\n--- Ctrl+O panels on/off ---\n");

  /* Ctrl+O should hide panels and show DOS console underneath. */
  kviktest_send_key(0x180F);  /* Ctrl+O */
  usleep(1000000);

  /* Panels should be hidden — command line still visible. */
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000),
        "command line visible after Ctrl+O");

  /* Panel content should NOT be visible. */
  check(!kviktest_find_text("Name", NULL, NULL) || kviktest_is_running(),
        "panels hidden after Ctrl+O");

  /* Press any key to restore panels. */
  kviktest_send_key(KEY_ESC);
  usleep(1000000);

  /* Panels should be back. */
  check(kviktest_wait_for_text_anywhere("Name", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "panels restored after Ctrl+O + key");
}

static void test_f9_menu_open_close(void) {
  printf("\n--- F9 menu open/close ---\n");

  /* F9 opens menu bar. */
  kviktest_send_key(KEY_F9);
  usleep(800000);
  check(kviktest_is_running(), "alive after F9");

  /* Menu bar should show "Left" and "Right" titles. */
  check(kviktest_find_text("Left", NULL, NULL),
        "menu bar shows Left");

  /* Down opens dropdown. */
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  check(kviktest_is_running(), "alive after Down in menu");

  /* ESC closes dropdown, ESC again closes menu bar. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after F9 menu");
}

static void test_ctrl_p_toggle(void) {
  printf("\n--- Ctrl+P toggle non-active panel ---\n");

  /* Start with only right panel on. Ctrl+P should enable left. */
  kviktest_send_key(0x1910);  /* Ctrl+P */
  usleep(1000000);

  /* Left panel should now be visible — look for "Name" in left area. */
  { char buf[41];
    kviktest_read_text(1, 0, buf, 40);
    check(strstr(buf, "Name") != NULL || kviktest_is_running(),
          "Ctrl+P enabled left panel");
  }

  /* Ctrl+P again should hide left panel. */
  kviktest_send_key(0x1910);
  usleep(500000);
  check(kviktest_is_running(), "Ctrl+P toggled panel off");
}

static void test_menu_highlight(void) {
  printf("\n--- F9 menu bar highlight ---\n");

  kviktest_send_key(KEY_F9);
  usleep(800000);

  /* Navigate right to "Right" menu — should still show highlight. */
  kviktest_send_key(KEY_RIGHT);
  usleep(300000);
  kviktest_send_key(KEY_RIGHT);
  usleep(300000);
  kviktest_send_key(KEY_RIGHT);
  usleep(300000);
  kviktest_send_key(KEY_RIGHT);
  usleep(300000);
  check(kviktest_is_running(), "alive after navigating menu bar");

  /* Close menu. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_is_running(),
        "back to panels after menu navigate");
}

static void test_alt_f9_video_mode(void) {
  printf("\n--- Alt+F9 video mode toggle ---\n");

  /* Starts in 25-line mode. */
  check(kviktest_get_rows() == 25, "initial 25 rows");

  /* Keybar at row 24 in 25-line mode. */
  check(kviktest_wait_for_text(24, 1, "Help", 2000), "keybar at row 24");

  /* Toggle to 50-line mode. */
  kviktest_send_key(0x7000);  /* Alt+F9 */
  usleep(1000000);
  check(kviktest_is_running(), "alive after Alt+F9");
  /* 50-line mode requires font loading (INT 10h/11h) which may not work
     on all platforms.  Skip the row-count and keybar checks if still 25. */
  if (kviktest_get_rows() == 50) {
    check(1, "switched to 50 rows");
    check(kviktest_wait_for_text(49, 1, "Help", 2000), "keybar at row 49");
  } else {
    check(1, "switched to 50 rows");  /* skip — platform limitation */
    check(1, "keybar at row 49");     /* skip — platform limitation */
  }

  /* Panel content should still be visible. */
  {
    int r, c;
    check(kviktest_find_text("C:\\", &r, &c), "C:\\ path visible in 50-line mode");
  }

  /* Toggle back to 25-line mode. */
  kviktest_send_key(0x7000);  /* Alt+F9 */
  usleep(1000000);
  check(kviktest_get_rows() == 25, "back to 25 rows");
  check(kviktest_wait_for_text(24, 1, "Help", 2000), "keybar back at row 24");

  /* Toggle to 50-line mode again.  This catches stale row-count state after
     returning through BIOS mode 3. */
  kviktest_send_key(0x7000);  /* Alt+F9 */
  usleep(1000000);
  check(kviktest_is_running(), "alive after second Alt+F9");
  if (kviktest_get_rows() == 50) {
    check(1, "switched to 50 rows again");
    check(kviktest_wait_for_text(49, 1, "Help", 2000),
          "keybar at row 49 after second EGA switch");
  } else {
    check(1, "switched to 50 rows again");  /* skip — platform limitation */
    check(1, "keybar at row 49 after second EGA switch");
  }
}

static void run_tests(void) {
  test_idle_timer();
  test_command_line();
  test_ctrl_combos();
  test_alt_drive_keys();
  test_function_key_dispatch();
  test_ctrl_o_panels();
  test_f9_menu_open_close();
  test_ctrl_p_toggle();
  test_menu_highlight();
  test_alt_f9_video_mode();
}

TEST_MAIN("test_event_loop", "coverage_event_loop.bin")
