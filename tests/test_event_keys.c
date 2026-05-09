/*
 * test_event_keys.c: exercise untested key combinations in the event loop.
 *
 * Targets untested paths in:
 *   - event_loop: Ctrl+Z, Ctrl+Q, Shift+F10, speed search edges
 *   - dispatch_file_list_input: Ctrl+[, Ctrl+], Ctrl+Enter
 */
#include "test_common.h"

#define KEY_INSERT   0x5200
#define KEY_CTRL_Z   0x2C1A
#define KEY_CTRL_Q   0x1011
#define KEY_CTRL_P   0x1910
#define KEY_CTRL_Y   0x1519
#define KEY_SHIFT_F10 0x5D00
#define KEY_CTRL_LBRACE 0x1A1B
#define KEY_CTRL_RBRACE 0x1B1D
#define KEY_CTRL_ENTER 0x1C0A
#define KEY_ALT_H    0x2300
#define KEY_BACKSPACE 0x0E08
#define KEY_SHIFT_TAB 0x0F00

/* ---- Ctrl+Z: toggle tree on inactive panel ---- */
static void test_ctrl_z_tree_toggle(void) {
  printf("\n--- Ctrl+Z tree toggle ---\n");

  /* Enable both panels with Ctrl+P. */
  kviktest_send_key(KEY_CTRL_P);
  usleep(500000);

  /* Send Ctrl+Z to toggle inactive panel to tree mode. */
  kviktest_send_key(KEY_CTRL_Z);
  usleep(500000);

  check(kviktest_is_running(), "alive after Ctrl+Z tree toggle");

  /* Toggle back. */
  kviktest_send_key(KEY_CTRL_Z);
  usleep(500000);

  check(kviktest_is_running(), "alive after Ctrl+Z toggle back");

  /* Restore single panel mode. */
  kviktest_send_key(KEY_CTRL_P);
  usleep(500000);
}

/* ---- Ctrl+Z with single panel (no-op path) ---- */
static void test_ctrl_z_single_panel(void) {
  printf("\n--- Ctrl+Z single panel ---\n");

  /* Only right panel is on — Ctrl+Z should be no-op or handled gracefully. */
  kviktest_send_key(KEY_CTRL_Z);
  usleep(500000);

  check(kviktest_is_running(), "alive after Ctrl+Z single panel");
}

/* ---- Ctrl+Q: quote next character ---- */
static void test_ctrl_q_quote(void) {
  printf("\n--- Ctrl+Q quote char ---\n");

  /* Clear cmdline. */
  kviktest_send_key(KEY_CTRL_Y);
  usleep(300000);

  /* Ctrl+Q enters "quote next char" mode.
     Send a regular letter after — it should be inserted literally. */
  kviktest_send_key(KEY_CTRL_Q);
  usleep(300000);
  kviktest_send_key(0x2C1A); /* Ctrl+Z — normally toggles tree, but quoted → literal */
  usleep(500000);

  check(kviktest_is_running(), "alive after Ctrl+Q quote");

  /* Clear cmdline. */
  kviktest_send_key(KEY_CTRL_Y);
  usleep(300000);
}

/* ---- Shift+F10: repeat last menu action ---- */
static void test_shift_f10_repeat(void) {
  printf("\n--- Shift+F10 repeat menu ---\n");

  /* Use Ctrl+F3 (sort by name) as a safe, repeatable menu action.
     Ctrl+F3 dispatches through the menu system and sets g_last_menu_action. */
  kviktest_send_key(0x6000); /* Ctrl+F3 = sort by Name */
  usleep(500000);

  /* Shift+F10 should repeat the sort-by-name action (safe no-op). */
  kviktest_send_key(KEY_SHIFT_F10);
  usleep(500000);

  check(kviktest_is_running(), "alive after Shift+F10 repeat");
}

/* ---- Speed search: Alt+letter ---- */
static void test_speed_search_basic(void) {
  printf("\n--- Speed search basic ---\n");

  /* Alt+H to search for files starting with 'H'. */
  kviktest_send_key(KEY_ALT_H);
  usleep(1000000);

  /* Info bar should show a file starting with H (HELLO.TXT). */
  check(kviktest_find_text("HELLO", NULL, NULL) ||
        kviktest_find_text("hello", NULL, NULL),
        "speed search found H file");

  /* ESC to close speed search. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  check(kviktest_is_running(), "alive after speed search");
}

/* ---- Speed search: backspace to close ---- */
static void test_speed_search_backspace(void) {
  printf("\n--- Speed search backspace ---\n");

  kviktest_send_key(KEY_ALT_H);
  usleep(500000);

  /* Backspace should delete the search char and close search. */
  kviktest_send_key(KEY_BACKSPACE);
  usleep(500000);

  check(kviktest_is_running(), "alive after speed search backspace");
}

/* ---- Speed search: Ctrl+Enter for next match ---- */
static void test_speed_search_ctrl_enter(void) {
  printf("\n--- Speed search Ctrl+Enter ---\n");

  kviktest_send_key(KEY_ALT_H);
  usleep(500000);

  /* Ctrl+Enter advances to next match. */
  kviktest_send_key(KEY_CTRL_ENTER);
  usleep(500000);

  check(kviktest_is_running(), "alive after Ctrl+Enter in search");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
}

/* ---- Ctrl+[ insert left panel path ---- */
static void test_ctrl_lbrace(void) {
  printf("\n--- Ctrl+[ insert path ---\n");

  /* Enable both panels. */
  kviktest_send_key(KEY_CTRL_P);
  usleep(500000);

  /* Clear cmdline. */
  kviktest_send_key(KEY_CTRL_Y);
  usleep(200000);

  /* Ctrl+[ inserts left panel path. */
  kviktest_send_key(KEY_CTRL_LBRACE);
  usleep(500000);

  /* Read cmdline row — should contain path text. */
  char buf[81];
  kviktest_read_text(23, 0, buf, 80);
  check(buf[0] != '\0', "cmdline has content after Ctrl+[");

  check(kviktest_is_running(), "alive after Ctrl+[");

  /* Clear and restore single panel. */
  kviktest_send_key(KEY_CTRL_Y);
  usleep(200000);
  kviktest_send_key(KEY_CTRL_P);
  usleep(500000);
}

/* ---- Ctrl+] insert right panel path ---- */
static void test_ctrl_rbrace(void) {
  printf("\n--- Ctrl+] insert path ---\n");

  /* Clear cmdline. */
  kviktest_send_key(KEY_CTRL_Y);
  usleep(200000);

  /* Ctrl+] inserts right panel path. */
  kviktest_send_key(KEY_CTRL_RBRACE);
  usleep(500000);

  char buf[81];
  kviktest_read_text(23, 0, buf, 80);
  check(buf[0] != '\0', "cmdline has content after Ctrl+]");

  check(kviktest_is_running(), "alive after Ctrl+]");

  kviktest_send_key(KEY_CTRL_Y);
  usleep(200000);
}

/* ---- Ctrl+Enter: insert filename to cmdline ---- */
static void test_ctrl_enter_filename(void) {
  printf("\n--- Ctrl+Enter insert filename ---\n");

  if (test_is_vc_499()) {
    /* 4.99.09 doesn't bind Ctrl+Enter to "insert filename": the panel
     * key dispatcher treats it as a no-op. Ctrl+J/Ctrl+M and the bare
     * Enter key are the documented bindings for that action in 4.99. */
    check(1, "skipped (Ctrl+Enter unbound in 4.99)");
    return;
  }

  /* Navigate to a known file. */
  navigate_to("hello", "HELLO");

  /* Clear cmdline. */
  kviktest_send_key(KEY_CTRL_Y);
  usleep(200000);

  /* Ctrl+Enter inserts the filename under cursor. */
  kviktest_send_key(KEY_CTRL_ENTER);
  usleep(500000);

  /* Check cmdline contains the filename. */
  char buf[81];
  kviktest_read_text(23, 0, buf, 80);
  check(strstr(buf, "HELLO") != NULL || strstr(buf, "hello") != NULL,
        "filename inserted to cmdline");

  check(kviktest_is_running(), "alive after Ctrl+Enter filename");

  kviktest_send_key(KEY_CTRL_Y);
  usleep(200000);
}

static void run_tests(void) {
  test_ctrl_z_tree_toggle();
  test_ctrl_z_single_panel();
  test_ctrl_q_quote();
  test_shift_f10_repeat();
  test_speed_search_basic();
  test_speed_search_backspace();
  test_speed_search_ctrl_enter();
  test_ctrl_lbrace();
  test_ctrl_rbrace();
  test_ctrl_enter_filename();
}

TEST_MAIN("test_event_keys", "coverage_event_keys.bin")
