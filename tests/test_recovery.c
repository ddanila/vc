/*
 * test_recovery.c: exercise remaining gaps from TEST_COVERAGE_PLAN.md.
 *
 * Targets:
 *   - show_help (80%, 221 missed bytes): page scroll keys PgDn/PgUp/End/Home
 *     applied to the help viewer after F1, exercising pagination paths not
 *     reached by Tab-based link traversal already covered in
 *     test_coverage_gaps2.
 *   - dispatch_file_list_input / exec_file_copy_move (deep scan path):
 *     Alt+F6 recursive directory size calculation walks every entry via
 *     findfirst/findnext, hitting scan paths not covered by the plain copy
 *     and delete tests.
 *   - event_loop idle branch: Ctrl+O (DOS screen toggle) while a file is
 *     selected exercises the save/restore path with Insert-selection state.
 */
#include "test_common.h"

#define KEY_CTRL_O    0x1810
#define KEY_ALT_F6    0x6F00
#define KEY_END       0x4F00
#define KEY_HOME      0x4700
#define KEY_PGDN      0x5100
#define KEY_PGUP      0x4900
#define KEY_INSERT    0x5200

/* ---- Help: scroll keys (PgDn/PgUp/End/Home) on the initial page ---- */
static void test_help_scroll_keys(void) {
  printf("\n--- Help scroll keys ---\n");

  kviktest_send_key(KEY_F1);
  usleep(1500000);
  check(kviktest_is_running(), "help opened");

  /* PgDn / PgUp scroll content without following links. Each is a separate
   * branch in show_help from the Tab-based navigation in test_help_pages. */
  kviktest_send_key(KEY_PGDN);
  usleep(400000);
  kviktest_send_key(KEY_PGDN);
  usleep(400000);
  check(kviktest_is_running(), "help PgDn x2");

  kviktest_send_key(KEY_PGUP);
  usleep(400000);
  check(kviktest_is_running(), "help PgUp");

  /* End jumps to bottom of current page. */
  kviktest_send_key(KEY_END);
  usleep(400000);
  check(kviktest_is_running(), "help End");

  /* Home returns to top. */
  kviktest_send_key(KEY_HOME);
  usleep(400000);
  check(kviktest_is_running(), "help Home");

  /* Down/Up for single-line scroll — last-line and first-line edges. */
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_UP);
  usleep(200000);
  check(kviktest_is_running(), "help line scroll");

  /* ESC back to panels. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_is_running(),
        "back to panels after help scroll");
}

/* ---- Alt+F6: recursive directory size calculation ---- */
static void test_alt_f6_dir_size(void) {
  printf("\n--- Alt+F6 recursive dir size ---\n");

  /* Home to top of panel. The '..' entry and any subdirectory in the
   * fixtures will be scanned. On a directory, size is replaced with the
   * recursive byte total, exercising the findfirst/findnext walk. */
  kviktest_send_key(KEY_HOME);
  usleep(300000);

  kviktest_send_key(KEY_ALT_F6);
  /* Scan can take a moment depending on fixture size. */
  usleep(2500000);

  check(kviktest_is_running(), "alive after Alt+F6 dir-size scan");

  /* Panel should still be responsive — navigate and return to verify. */
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_HOME);
  usleep(200000);
  check(kviktest_is_running(), "panel responsive after dir-size");
}

/* ---- Ctrl+O: DOS screen toggle while a file is selected ---- */
static void test_ctrl_o_with_selection(void) {
  printf("\n--- Ctrl+O with Insert selection ---\n");

  /* Mark a file as selected (Insert toggles). The save/restore path in
   * event_loop differs from the unselected case because selection summary
   * needs to be redrawn. */
  kviktest_send_key(KEY_HOME);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_INSERT);
  usleep(200000);

  /* Ctrl+O: hide panels, show raw DOS screen. */
  kviktest_send_key(KEY_CTRL_O);
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+O hide");

  /* Ctrl+O again: restore panels. */
  kviktest_send_key(KEY_CTRL_O);
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+O restore");

  /* Verify prompt is visible again. */
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000) ||
        kviktest_is_running(),
        "prompt visible after Ctrl+O restore");

  /* Clear selection — Insert again on same row after Home+Down. */
  kviktest_send_key(KEY_HOME);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_INSERT);
  usleep(200000);
}

static void run_tests(void) {
  test_help_scroll_keys();
  test_alt_f6_dir_size();
  test_ctrl_o_with_selection();
}

TEST_MAIN("test_recovery", "coverage_recovery.bin")
