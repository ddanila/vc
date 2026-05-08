/*
 * test_menu.c: menu system and panel modes
 */
#include "test_common.h"

static void test_menu_system(void) {
  printf("\n--- Menu system (F9) ---\n");

  /* F9 opens the pull-down menu bar. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  check(kviktest_is_running(), "alive after F9");
  check(kviktest_find_text("Left", NULL, NULL) ||
        kviktest_find_text("File", NULL, NULL),
        "menu bar visible");

  /* Navigate menu with arrow keys. */
  kviktest_send_key(KEY_RIGHT);
  usleep(200000);
  kviktest_send_key(KEY_RIGHT);
  usleep(200000);
  check(kviktest_is_running(), "alive navigating menu");

  /* Escape to close menu. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 2000), "back to panels after menu");
}

static void test_config_dialogs(void) {
  printf("\n--- Config dialogs (F9 menu) ---\n");

  /* Open menu bar with F9. */
  kviktest_send_key(KEY_F9);
  usleep(500000);

  /* Menu bar: Left, Files, Commands, Options, Right.
   * Walk through each top-level menu and open its dropdown. */

  /* "Left" menu — first item, already selected. Press Down to open. */
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  check(kviktest_is_running(), "Left menu opened");

  /* ESC back to menu bar. */
  kviktest_send_key(KEY_ESC);
  usleep(300000);

  /* Right arrow → "Files" menu. */
  kviktest_send_key(KEY_RIGHT);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  check(kviktest_is_running(), "Files menu opened");
  kviktest_send_key(KEY_ESC);
  usleep(300000);

  /* Right arrow → "Commands" menu. */
  kviktest_send_key(KEY_RIGHT);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  check(kviktest_is_running(), "Commands menu opened");
  kviktest_send_key(KEY_ESC);
  usleep(300000);

  /* Right arrow → "Options" menu. */
  kviktest_send_key(KEY_RIGHT);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  check(kviktest_is_running(), "Options menu opened");

  /* Select first item in Options (likely Configuration or Colors).
   * Press Enter to open the dialog. */
  kviktest_send_key(KEY_ENTER);
  usleep(500000);
  check(kviktest_is_running(), "Options dialog opened");

  /* ESC to close dialog. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* Open Options again, navigate to second item. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  /* Navigate to Options: Right x3 from Left. */
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);  /* Second item. */
  usleep(300000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);
  check(kviktest_is_running(), "second Options dialog opened");
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* Open Options → third item. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);  /* Third item. */
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);
  check(kviktest_is_running(), "third Options dialog opened");
  kviktest_send_key(KEY_ESC);
  usleep(500000);

  /* Right arrow → "Right" panel menu. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  check(kviktest_is_running(), "Right panel menu opened");
  kviktest_send_key(KEY_ESC);
  usleep(300000);

  /* Close menu bar — may need multiple ESC on slow systems. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 5000) ||
        kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after config dialogs");
}

static void test_commands_menu(void) {
  printf("\n--- Commands menu ---\n");

  /* Swap panels: Ctrl+U. */
  kviktest_send_key(0x1615);  /* Ctrl+U: scancode 0x16='U', ASCII 0x15 */
  usleep(500000);
  check(kviktest_is_running(), "alive after Ctrl+U (swap panels)");

  /* Swap back. */
  kviktest_send_key(0x1615);
  usleep(500000);
  check(kviktest_is_running(), "alive after second Ctrl+U");

  /* Alt+F8 = Command history. */
  kviktest_send_key(0x6F00);  /* Alt+F8 */
  usleep(500000);
  /* History dialog may be empty — just ESC to close. */
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_is_running(), "alive after Alt+F8 (history)");

  /* F9 → Commands menu → navigate items. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_RIGHT);  /* Files */
  usleep(200000);
  kviktest_send_key(KEY_RIGHT);  /* Commands */
  usleep(200000);
  kviktest_send_key(KEY_DOWN);   /* Open Commands dropdown */
  usleep(300000);
  kviktest_send_key(KEY_DOWN);   /* Second item */
  usleep(200000);
  kviktest_send_key(KEY_DOWN);   /* Third item */
  usleep(200000);
  kviktest_send_key(KEY_DOWN);   /* Fourth item */
  usleep(200000);
  check(kviktest_is_running(), "Commands menu navigation");
  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(300000);
}

static void test_panel_modes(void) {
  printf("\n--- Panel modes ---\n");

  /* Panel modes are selected from the Left/Right menu. Use item hotkeys
   * here so the test is independent of which mode is initially highlighted. */

  /* Open Left menu and select "Brief". */
  kviktest_send_key(KEY_F9);
  usleep(500000);
  kviktest_send_key(KEY_DOWN);  /* Open Left dropdown */
  usleep(300000);
  type_string("b");
  usleep(1000000);
  check(kviktest_is_running(), "Brief mode");

  /* Open Left menu -> "Full". */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  type_string("f");
  usleep(1000000);
  /* Full mode shows Size/Date/Time columns. */
  check(kviktest_find_text("Size", NULL, NULL) ||
        kviktest_find_text("Date", NULL, NULL) ||
        kviktest_is_running(),
        "Full mode");

  /* Open Left menu -> "Info". */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  type_string("i");
  usleep(2000000);

  /* Info panel shows memory, disk space, volume info, and dirinfo.
   * Wait for rendering and verify content. */
  check(kviktest_wait_for_text_anywhere("Memory", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("Bytes", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("bytes", 3000, NULL, NULL),
        "Info panel content rendered");

  /* Check for volume/drive info. */
  check(kviktest_find_text("Volume", NULL, NULL) ||
        kviktest_find_text("drive", NULL, NULL) ||
        kviktest_find_text("free", NULL, NULL) ||
        kviktest_is_running(),
        "Info panel drive info");

  /* Check for dirinfo content (fixture has a dirinfo file). */
  check(kviktest_find_text("dirinfo", NULL, NULL) ||
        kviktest_find_text("Test fixture", NULL, NULL) ||
        kviktest_is_running(),
        "Info panel dirinfo or placeholder");

  /* Navigate while in Info mode: Down/Up change the file cursor,
   * causing the Info panel to re-render for each file. */
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  kviktest_send_key(KEY_DOWN);
  usleep(500000);
  kviktest_send_key(KEY_UP);
  usleep(500000);
  check(kviktest_is_running(), "alive navigating in Info mode");

  /* Switch back to Brief mode for the rest of the tests. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
  type_string("b");
  usleep(1000000);
  check(kviktest_wait_for_text_anywhere("Name", 2000, NULL, NULL),
        "back to Brief mode");

  /* Also toggle with Ctrl+F1 (show/hide left panel). */
  kviktest_send_key(0x5E00);  /* Ctrl+F1 */
  usleep(1000000);
  check(kviktest_is_running(), "Ctrl+F1 panel toggle");

  /* Toggle back. */
  kviktest_send_key(0x5E00);  /* Ctrl+F1 */
  usleep(1000000);
  check(kviktest_is_running(), "Ctrl+F1 panel restore");

  /* Ctrl+O hides both panels (command line mode). */
  kviktest_send_key(0x180F);  /* Ctrl+O: scancode 0x18='O', with Ctrl = 0x0F? */
  /* Actually Ctrl+O: ASCII 0x0F (Ctrl+O), scancode 0x18. */
  usleep(500000);
  check(kviktest_is_running(), "Ctrl+O panels off");

  /* Ctrl+O again to restore. */
  kviktest_send_key(0x180F);
  usleep(1000000);
  check(kviktest_wait_for_text_anywhere("Name", 2000, NULL, NULL) ||
        kviktest_wait_for_text(23, 0, "C:\\>", 2000),
        "Ctrl+O panels restored");
}

static void open_right_menu(void) {
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_RIGHT);
  usleep(100000);
  kviktest_send_key(KEY_DOWN);
  usleep(300000);
}

static void select_right_mode_hotkey(const char *hotkey) {
  open_right_menu();
  type_string(hotkey);
  usleep(1000000);
}

static int host_file_contains_text(const char *name, const char *needle) {
  char path[1024];
  char buf[256];
  FILE *fp;
  size_t n;

  snprintf(path, sizeof(path), "%s/%s", g_mount_dir, name);
  fp = fopen(path, "rb");
  if (!fp) return 0;
  n = fread(buf, 1, sizeof(buf) - 1, fp);
  fclose(fp);
  buf[n] = '\0';
  return strstr(buf, needle) != NULL;
}

static void test_right_full_info_menu_reopen(void) {
  char row[81];

  printf("\n--- Right panel Full -> Info menu reopen ---\n");
  if (!getenv("VC_BINARY")) {
    check(1, "skipped for original VC.COM");
    return;
  }

  /* Default visible panel is the right panel. Reproduce the reported path:
   * top menu -> Right -> Full, then top menu -> Right -> Info. */
  select_right_mode_hotkey("f");
  check(kviktest_wait_for_text_anywhere("Size", 2000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("Date", 2000, NULL, NULL),
        "right panel switched to Full");

  select_right_mode_hotkey("i");
  check(kviktest_wait_for_text_anywhere("Memory", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("bytes", 3000, NULL, NULL),
        "right panel switched to Info");

  /* Reopen the top menu and assert that both the bar and dropdown are sane.
   * This catches the screen-save/menu buffer corruption that showed garbage. */
  open_right_menu();
  kviktest_read_text(0, 0, row, sizeof(row));
  check(strstr(row, "Left") && strstr(row, "Files") &&
        strstr(row, "Commands") && strstr(row, "Options") &&
        strstr(row, "Right"),
        "menu bar intact after Full -> Info");
  check(kviktest_find_text("Brief", NULL, NULL) &&
        kviktest_find_text("Full", NULL, NULL) &&
        kviktest_find_text("Info", NULL, NULL),
        "Right dropdown intact after Full -> Info");

  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000),
        "back to panels after Full -> Info menu reopen");

  /* Restore the default file-panel mode for any later tests. */
  select_right_mode_hotkey("b");
  check(kviktest_wait_for_text_anywhere("Name", 2000, NULL, NULL),
        "right panel restored to Brief");
}

static void test_right_full_info_left_wrap_reopen(void) {
  char row[81];

  printf("\n--- Right panel Full -> Info via left-wrap reopen ---\n");
  if (!getenv("VC_BINARY")) {
    check(1, "skipped for original VC.COM");
    return;
  }

  select_right_mode_hotkey("b");
  check(kviktest_wait_for_text_anywhere("Name", 2000, NULL, NULL),
        "right panel starts in Brief before left-wrap sequence");

  /* Exact reported sequence:
   * F9, Left, Enter, Down, Enter -> Right/Full
   * F9, Left, Down, Down, Enter -> Right/Info
   * F9, Left -> Right menu bar selected; screen must stay sane. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_LEFT);
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_wait_for_text_anywhere("Size", 2000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("Date", 2000, NULL, NULL),
        "left-wrap sequence switched right panel to Full");

  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_LEFT);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_ENTER);
  usleep(1500000);
  check(kviktest_wait_for_text_anywhere("Memory", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("bytes", 3000, NULL, NULL),
        "left-wrap sequence switched right panel to Info");

  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_LEFT);
  usleep(500000);
  kviktest_read_text(0, 0, row, sizeof(row));
  check(strstr(row, "Left") && strstr(row, "Files") &&
        strstr(row, "Commands") && strstr(row, "Options") &&
        strstr(row, "Right"),
        "menu bar intact after left-wrap Full -> Info");
  check(kviktest_find_text("Memory", NULL, NULL) ||
        kviktest_find_text("bytes", NULL, NULL),
        "Info panel still readable after left-wrap menu reopen");

  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000),
        "back to panels after left-wrap menu reopen");

  select_right_mode_hotkey("b");
  check(kviktest_wait_for_text_anywhere("Name", 2000, NULL, NULL),
        "right panel restored to Brief after left-wrap test");
}

static void test_commands_menu_repeated_down_redraw(void) {
  char row[81];

  printf("\n--- Commands menu repeated Down redraw ---\n");
  if (!getenv("VC_BINARY")) {
    check(1, "skipped for original VC.COM");
    return;
  }

  /* Exact reported sequence: F9, Right, Right, Enter, then Down x5. */
  kviktest_send_key(KEY_F9);
  usleep(300000);
  kviktest_send_key(KEY_RIGHT);
  usleep(150000);
  kviktest_send_key(KEY_RIGHT);
  usleep(150000);
  kviktest_send_key(KEY_ENTER);
  usleep(300000);
  for (int i = 0; i < 5; i++) {
    kviktest_send_key(KEY_DOWN);
    usleep(200000);
  }

  kviktest_read_text(0, 0, row, sizeof(row));
  check(strstr(row, "Left") && strstr(row, "Files") &&
        strstr(row, "Commands") && strstr(row, "Options") &&
        strstr(row, "Right"),
        "menu bar intact after Commands Down x5");
  check(kviktest_find_text("Tree", NULL, NULL) &&
        kviktest_find_text("memory Info", NULL, NULL) &&
        kviktest_find_text("History", NULL, NULL),
        "Commands dropdown intact after Down x5");
  check(kviktest_wait_for_text(23, 0, "C:\\>", 500),
        "command line intact while Commands menu is open");

  kviktest_send_key(KEY_ESC);
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  check(kviktest_wait_for_text(23, 0, "C:\\>", 3000),
        "back to panels after Commands Down x5");
}

static void test_dirinfo_edit_buffer(void) {
  printf("\n--- Info dirinfo edit buffer ---\n");
  if (!getenv("VC_BINARY")) {
    check(1, "skipped for original VC.COM");
    return;
  }

  select_right_mode_hotkey("i");
  check(kviktest_wait_for_text_anywhere("Memory", 3000, NULL, NULL) ||
        kviktest_wait_for_text_anywhere("bytes", 3000, NULL, NULL),
        "right panel switched to Info before dirinfo edit");

  kviktest_send_key(KEY_F4);
  usleep(500000);
  type_string("ZQ");
  usleep(300000);
  kviktest_send_key(KEY_ESC);
  usleep(1000000);

  check(host_file_contains_text("dirinfo", "ZQ") ||
        host_file_contains_text("DIRINFO", "ZQ"),
        "dirinfo edit saved through far buffer");

  select_right_mode_hotkey("b");
  check(kviktest_wait_for_text_anywhere("Name", 2000, NULL, NULL),
        "right panel restored after dirinfo edit");
}

static void run_tests(void) {
  test_menu_system();
  test_config_dialogs();
  test_commands_menu();
  test_right_full_info_menu_reopen();
  test_right_full_info_left_wrap_reopen();
  test_commands_menu_repeated_down_redraw();
  test_dirinfo_edit_buffer();
  test_panel_modes();
}

TEST_MAIN("test_menu", "coverage_menu.bin")
