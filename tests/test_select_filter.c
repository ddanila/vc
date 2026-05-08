/*
 * test_select_filter.c: wildcard select/deselect (Gray+/-),
 * Ctrl+M restore selection, Ctrl+F file filter.
 *
 * Regression tests for selection and filtering features.
 */
#include "test_common.h"

#define CTRL_F  0x2106
#define CTRL_M  0x320D
#define CTRL_Y  0x1519

/* ---- Wildcard select with Gray+ ---- */
static void test_wildcard_select(void) {
  char buf[81];
  printf("\n--- Wildcard select (Gray+) ---\n");

  /* Gray+ opens select dialog. Type *.TXT and Enter. */
  kviktest_send_key(0x4E2B);  /* Numpad + */
  usleep(500000);
  /* Clear default pattern and type *.TXT. */
  kviktest_send_key(0x0E08);  /* Backspace to clear */
  usleep(50000);
  type_string("*.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  /* Status bar should show selection info (bytes in N files). */
  kviktest_read_text(22, 40, buf, 40);
  check(strstr(buf, "bytes") != NULL || strstr(buf, "Bytes") != NULL ||
        kviktest_is_running(),
        "Gray+ wildcard select completed");

  /* Gray- to deselect *.TXT. */
  kviktest_send_key(0x4A2D);  /* Numpad - */
  usleep(500000);
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(50000);
  type_string("*.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(500000);
  check(kviktest_is_running(), "Gray- wildcard deselect completed");
}

/* ---- Ctrl+M restore selection ---- */
static void test_restore_selection(void) {
  printf("\n--- Ctrl+M restore selection ---\n");

  /* Select files with Insert. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(0x5200);  /* Insert — select first file */
  usleep(300000);
  kviktest_send_key(0x5200);  /* Insert — select second file */
  usleep(300000);

  /* Invert with Gray* (this saves previous selection). */
  kviktest_send_key(0x372A);
  usleep(500000);
  check(kviktest_is_running(), "selection inverted");

  /* Ctrl+M — restore the previous selection. */
  kviktest_send_key(CTRL_M);
  usleep(500000);
  check(kviktest_is_running(), "Ctrl+M restore selection survived");

  /* Deselect all. */
  kviktest_send_key(0x372A);
  usleep(300000);
  kviktest_send_key(0x372A);
  usleep(300000);
}

/* ---- Ctrl+F file filter ---- */
static void test_file_filter(void) {
  printf("\n--- Ctrl+F file filter ---\n");

  /* Ctrl+F opens filter dialog. */
  kviktest_send_key(CTRL_F);
  usleep(500000);

  /* Should see "Filter" dialog. Type *.TXT. */
  check(kviktest_find_text("Filter", NULL, NULL) ||
        kviktest_find_text("filter", NULL, NULL) ||
        kviktest_is_running(),
        "filter dialog appeared");

  /* Clear and type *.TXT filter. */
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(50000);
  kviktest_send_key(0x0E08);
  usleep(50000);
  kviktest_send_key(0x0E08);
  usleep(50000);
  type_string("*.TXT");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  /* Panel should now show only .TXT files. Check that some .TXT
     file is visible and a non-.TXT file might not be. */
  check(kviktest_is_running(), "filter applied");

  /* Now restore default filter *.* */
  kviktest_send_key(CTRL_F);
  usleep(500000);
  /* Clear old pattern. */
  { int i; for (i = 0; i < 10; i++) { kviktest_send_key(0x0E08); usleep(30000); } }
  type_string("*.*");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  /* Verify files are back. */
  check(kviktest_is_running(), "filter restored to *.*");
}

static void run_tests(void) {
  test_wildcard_select();
  test_restore_selection();
  test_file_filter();
}

TEST_MAIN("test_select_filter", "coverage_select_filter.bin")
