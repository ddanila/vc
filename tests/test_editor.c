/*
 * test_editor.c: editor operations (F4, cursor, search, save)
 */
#include "test_common.h"

static void test_file_editor(void) {
  printf("\n--- File editor (F4) ---\n");

  /* Navigate to HELLO.TXT and open editor. */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F4);

  /* Editor changes the function key bar — look for "Save" or "Q&Save". */
  check(kviktest_wait_for_text_anywhere("Save", 3000, NULL, NULL),
        "F4 editor opened");

  /* File content should be visible. */
  check(kviktest_wait_for_text_anywhere("Hello World", 2000, NULL, NULL),
        "editor shows file content");

  /* Type some text. */
  type_string("Hi");
  usleep(300000);
  check(kviktest_is_running(), "alive after typing");

  /* Undo typing with Ctrl+Z (or just verify cursor moved). */
  /* Move cursor: Home, End, arrow keys. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(200000);
  kviktest_send_key(0x4F00);  /* End */
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_UP);
  usleep(200000);
  check(kviktest_is_running(), "alive after cursor movement");

  /* Search in editor (F7). */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  type_string("Hello");
  kviktest_send_key(KEY_ENTER);
  usleep(500000);
  check(kviktest_is_running(), "alive after editor search");

  /* ESC to close editor WITHOUT saving (discard changes). */
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* If editor asks "Save changes?" — press N or ESC to discard. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL) ||
        kviktest_wait_for_text(23, 0, "C:\\>", 2000),
        "back to panels after editor");
}

static void test_editor_ops(void) {
  printf("\n--- Editor operations ---\n");

  /* Open editor on HELLO.TXT. */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F4);
  usleep(1000000);

  /* Ctrl+Y = delete line. */
  kviktest_send_key(0x1519);  /* Ctrl+Y: scancode 0x15='Y', ASCII 0x19 */
  usleep(300000);
  check(kviktest_is_running(), "alive after Ctrl+Y (delete line)");

  /* F1 inside editor = context help. */
  kviktest_send_key(KEY_F1);
  usleep(1000000);
  check(kviktest_is_running(), "alive after F1 in editor");
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* Block select: Ctrl+K B (begin) + cursor move + Ctrl+K K (end). */
  kviktest_send_key(0x4700);  /* Home */
  usleep(200000);
  kviktest_send_key(0x300B);  /* Ctrl+K: scancode 0x25='K', ASCII 0x0B */
  usleep(200000);
  /* After Ctrl+K, VC waits for second key of chord. Send 'B' for block begin. */
  kviktest_send_key(0x3042);  /* 'B' */
  usleep(300000);

  /* Move right a few chars. */
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);

  /* Ctrl+K K = block end. */
  kviktest_send_key(0x300B);  /* Ctrl+K */
  usleep(200000);
  kviktest_send_key(0x254B);  /* 'K' */
  usleep(300000);
  check(kviktest_is_running(), "alive after block select");

  /* ESC to close editor without saving. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);  /* Discard changes if prompted. */
  usleep(500000);
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL) ||
        kviktest_wait_for_text(23, 0, "C:\\>", 2000),
        "back to panels after editor ops");
}

static void test_editor_save(void) {
  printf("\n--- Editor save (F2) ---\n");

  /* Navigate to HELLO.TXT and open editor. */
  navigate_to("hello", NULL);
  kviktest_send_key(KEY_F4);
  check(kviktest_wait_for_text_anywhere("Save", 3000, NULL, NULL),
        "editor opened for save test");

  /* Go to end of file and type "ZZ". */
  kviktest_send_key(0x4F00);  /* End */
  usleep(200000);
  kviktest_send_key(0x2C5A);  /* 'Z' */
  usleep(50000);
  kviktest_send_key(0x2C5A);  /* 'Z' */
  usleep(200000);

  /* F2 = Save. */
  kviktest_send_key(KEY_F2);
  usleep(1000000);
  check(kviktest_is_running(), "alive after F2 save");

  /* Undo changes: delete "ZZ" with two Backspaces, then save again. */
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(100000);
  kviktest_send_key(0x0E08);  /* Backspace */
  usleep(200000);
  kviktest_send_key(KEY_F2);
  usleep(1000000);
  check(kviktest_is_running(), "alive after undo save");

  /* Close editor — no changes, ESC should work directly. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after editor save");
}

static void run_tests(void) {
  test_file_editor();
  test_editor_ops();
  test_editor_save();
}

TEST_MAIN("test_editor", "coverage_editor.bin")
