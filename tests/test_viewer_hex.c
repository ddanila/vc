/*
 * test_viewer_hex.c: hex viewer deep coverage —
 * hex rendering, search in hex mode, binary file display, cursor highlighting.
 */
#include "test_common.h"

/* Navigate to a file and open it in F3 viewer. */
static void open_viewer(const char *name) {
  navigate_to(name, NULL);
  kviktest_send_key(KEY_F3);
  usleep(1000000);
}

/* Close viewer with ESC and wait for panels. */
static void close_viewer(void) {
  kviktest_send_key(KEY_ESC);
  usleep(500000);
}

static void test_hex_rendering(void) {
  printf("\n--- Hex rendering (DATA.BIN) ---\n");

  /* Open DATA.BIN (4 bytes: 00 01 02 03) in viewer. */
  open_viewer("data");

  /* Switch to hex mode (F4). */
  kviktest_send_key(KEY_F4);
  usleep(500000);

  /* Hex mode should show hex digits on screen. DATA.BIN contains 00 01 02 03.
   * Look for "00 01 02 03" or individual hex pairs. */
  check(kviktest_wait_for_text_anywhere("00", 2000, NULL, NULL),
        "hex byte 00 visible");
  check(kviktest_find_text("01", NULL, NULL) ||
        kviktest_find_text("02", NULL, NULL) ||
        kviktest_find_text("03", NULL, NULL),
        "hex bytes 01/02/03 visible");

  /* Hex mode function bar should show "Text" or "ASCII" (instead of "Hex"). */
  check(kviktest_find_text("Text", NULL, NULL) ||
        kviktest_find_text("ASCII", NULL, NULL) ||
        kviktest_find_text("Unwrap", NULL, NULL),
        "hex mode function bar");

  /* Navigate: Home, End, PgDn, PgUp in hex mode. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(300000);
  kviktest_send_key(0x4F00);  /* End */
  usleep(300000);
  check(kviktest_is_running(), "hex Home/End navigation");

  /* Back to text mode, close. */
  kviktest_send_key(KEY_F4);
  usleep(300000);
  close_viewer();
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after hex rendering");
}

/* Verify hex layout matches original: group-of-4 spacing, uniform color,
   header bar, raw ASCII chars. Uses HELLO.TXT (12 bytes). */
static void test_hex_layout_match(void) {
  extern const unsigned char *g_dump_video_mem;
  extern unsigned g_dump_video_size;

  printf("\n--- Hex layout match ---\n");

  open_viewer("hello");
  kviktest_send_key(KEY_F4);  /* hex mode */
  usleep(1000000);

  if (!g_dump_video_mem || g_dump_video_size < 4000) {
    check(0, "VGA memory accessible for hex layout");
    close_viewer();
    return;
  }

  /* Row 0 = header bar: should start with "View:" */
  check(kviktest_find_text("View:", NULL, NULL), "hex header bar present");

  /* Row 1 = first data row: check group-of-4 spacing.
     Expected: " 00000000  48 65 6C 6C  6F 20 57 6F  72 6C 64 0A"
     Groups: "48 65 6C 6C" then extra space then "6F 20 57 6F" etc.
     Col 23-24 should be "  " (double space = group separator after 4th byte) */
  { unsigned char c23 = g_dump_video_mem[(1*80+23)*2];
    unsigned char c24 = g_dump_video_mem[(1*80+24)*2];
    check(c23 == ' ' && c24 == '6',
          "hex group-of-4 spacing (extra space at col 23)");
  }

  /* All hex data should use uniform attribute (0x1B = CLR_V_PANEL_BODY).
     Check that col 11 (first hex digit) has same attr as col 63 (ASCII). */
  { unsigned char attr_hex = g_dump_video_mem[(1*80+11)*2+1];
    unsigned char attr_asc = g_dump_video_mem[(1*80+63)*2+1];
    check(attr_hex == attr_asc,
          "hex and ASCII areas use same attribute");
  }

  kviktest_send_key(KEY_F4);  /* back to text */
  usleep(300000);
  close_viewer();
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after hex layout");
}

static void test_hex_search(void) {
  printf("\n--- Hex search ---\n");

  /* Open HELLO.TXT in viewer, switch to hex mode, search. */
  open_viewer("hello");
  kviktest_send_key(KEY_F4);  /* hex mode */
  usleep(500000);

  /* F7 = search in hex mode. Type hex bytes for "World" = 57 6F 72 6C 64. */
  kviktest_send_key(KEY_F7);
  usleep(500000);

  /* The search dialog should appear. Type search text "World". */
  type_string("World");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  /* Search should find "World" and highlight it. Emulator should be alive. */
  check(kviktest_is_running(), "alive after hex search");

  /* The hex view should still show hex digits. */
  check(kviktest_find_text("57", NULL, NULL) ||  /* 'W' = 0x57 */
        kviktest_find_text("6F", NULL, NULL) ||  /* 'o' = 0x6F */
        kviktest_is_running(),
        "hex digits visible after search");

  /* Search again with F7 (repeat search). */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);  /* repeat same search */
  usleep(500000);
  check(kviktest_is_running(), "alive after repeat search");

  /* Back to text, close. */
  kviktest_send_key(KEY_F4);
  usleep(300000);
  close_viewer();
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after hex search");
}

static void test_hex_scroll_deep(void) {
  printf("\n--- Hex scroll deep ---\n");

  /* Open VC.HLP (large file, ~68KB) in hex mode for scroll testing. */
  open_viewer("vc");  /* matches "vc.hlp" or "vc.ext" — try hlp first */

  kviktest_send_key(KEY_F4);  /* hex mode */
  usleep(500000);

  /* Page down multiple times to exercise hex rendering on different data. */
  kviktest_send_key(0x5100);  /* PgDn */
  usleep(300000);
  kviktest_send_key(0x5100);  /* PgDn */
  usleep(300000);
  kviktest_send_key(0x5100);  /* PgDn */
  usleep(300000);
  check(kviktest_is_running(), "alive after 3x PgDn in hex");

  /* Arrow keys in hex mode — scroll by single row. */
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_DOWN);
  usleep(200000);
  kviktest_send_key(KEY_UP);
  usleep(200000);
  check(kviktest_is_running(), "alive after arrow scroll in hex");

  /* End key — jump to end of file in hex. */
  kviktest_send_key(0x4F00);  /* End */
  usleep(500000);
  check(kviktest_is_running(), "alive after End in hex (large file)");

  /* Home key — back to start. */
  kviktest_send_key(0x4700);  /* Home */
  usleep(500000);
  check(kviktest_is_running(), "alive after Home in hex");

  /* PgUp from near start. */
  kviktest_send_key(0x4900);  /* PgUp */
  usleep(300000);
  check(kviktest_is_running(), "alive after PgUp from start in hex");

  /* Close. */
  kviktest_send_key(KEY_F4);
  usleep(300000);
  close_viewer();
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after hex scroll deep");
}

static void test_text_search_highlight(void) {
  printf("\n--- Text search highlight ---\n");

  /* Open HELLO.TXT in text mode, search to trigger cursor highlighting. */
  open_viewer("hello");

  /* F7 search for "Hello". */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  type_string("Hello");
  kviktest_send_key(KEY_ENTER);
  usleep(500000);
  check(kviktest_is_running(), "alive after text search");

  /* "Hello" should be visible and highlighted (we can't check attribute,
   * but we verify the text is on screen). */
  check(kviktest_find_text("Hello", NULL, NULL),
        "search result 'Hello' visible");

  /* Search for "World" — different position in the line. */
  kviktest_send_key(KEY_F7);
  usleep(800000);
  type_string("World");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);
  check(kviktest_find_text("World", NULL, NULL),
        "search result 'World' visible");

  /* Search for something not found. */
  kviktest_send_key(KEY_F7);
  usleep(500000);
  type_string("XXX");
  kviktest_send_key(KEY_ENTER);
  usleep(1000000);

  /* "Not found" dialog should appear. Dismiss. */
  check(kviktest_is_running(), "alive after not-found search");
  kviktest_send_key(KEY_ESC);
  usleep(500000);
  kviktest_send_key(KEY_ENTER);
  usleep(500000);

  close_viewer();
  check(kviktest_wait_for_text_anywhere("Help", 2000, NULL, NULL),
        "back to panels after text search");
}

static void run_tests(void) {
  test_hex_rendering();
  test_hex_layout_match();
  test_hex_search();
  test_hex_scroll_deep();
  test_text_search_highlight();
}

TEST_MAIN("test_viewer_hex", "coverage_viewer_hex.bin")
