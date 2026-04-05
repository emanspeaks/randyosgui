#include <assert.h>
#include <stdio.h>
#include "randyosgui.h"

/* Minimal test harness â€” no external deps */

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST(name) static void test_##name(void)
#define RUN(name) do { \
    tests_run++; \
    printf("  %-40s", #name); \
    test_##name(); \
    tests_passed++; \
    printf("PASS\n"); \
} while (0)

/* draw-command conformance tests */
void drawspec_checkbox_exact(void);
void drawspec_radio_exact_contrast(void);
void drawspec_status_field_border(void);
void drawspec_sunken_panel_border(void);

/* =========================================================================
 * Tests
 * ========================================================================= */

TEST(init_shutdown) {
    RandyContext* ctx = randy_init();
    assert(ctx != NULL);
    randy_shutdown(ctx);
}

TEST(init_shutdown_null_safe) {
    randy_shutdown(NULL); /* must not crash */
}

TEST(result_strings) {
    assert(randy_result_string(RANDY_OK)                 != NULL);
    assert(randy_result_string(RANDY_ERR_NOMEM)          != NULL);
    assert(randy_result_string(RANDY_ERR_PLATFORM)       != NULL);
    assert(randy_result_string(RANDY_ERR_RENDERER)       != NULL);
    assert(randy_result_string(RANDY_ERR_INVALID_HANDLE) != NULL);
}

TEST(window_create_destroy) {
    RandyContext* ctx = randy_init();
    assert(ctx != NULL);

    RandyWindowDesc desc = {
        .title     = "test window",
        .width     = 640,
        .height    = 480,
        .resizable = false,
        .decorated = true,
    };
    RandyWindow* win = randy_window_create(ctx, &desc);
    assert(win != NULL);

    int w = 0, h = 0;
    randy_window_get_size(win, &w, &h);
    assert(w == 640);
    assert(h == 480);

    assert(!randy_window_should_close(win));

    randy_window_destroy(win);
    randy_shutdown(ctx);
}

TEST(widget_label) {
    RandyContext* ctx = randy_init();
    RandyWindowDesc desc = { .title="t", .width=100, .height=100 };
    RandyWindow* win = randy_window_create(ctx, &desc);
    assert(win != NULL);

    RandyWidgetId id = randy_label_create(win, "hello");
    assert(id != 0);

    randy_label_set_text(win, id, "updated");
    /* just checking it doesn't crash */

    randy_window_destroy(win);
    randy_shutdown(ctx);
}

static void button_cb(RandyWidgetId id, void* userdata) {
    (void)id;
    (*(int*)userdata)++;
}

TEST(widget_button_callback) {
    RandyContext* ctx = randy_init();
    RandyWindowDesc desc = { .title="t", .width=100, .height=100 };
    RandyWindow* win = randy_window_create(ctx, &desc);
    assert(win != NULL);

    RandyWidgetId id = randy_button_create(win, "click me");
    assert(id != 0);

    int fired = 0;
    randy_button_set_callback(win, id, button_cb, &fired);

    randy_window_destroy(win);
    randy_shutdown(ctx);
}

static void checkbox_cb(RandyWidgetId id, bool checked, void* userdata) {
    (void)id;
    (void)checked;
    int* toggles = (int*)userdata;
    (*toggles)++;
}

TEST(widget_checkbox_state) {
    RandyContext* ctx = randy_init();
    RandyWindowDesc desc = { .title="t", .width=180, .height=120 };
    RandyWindow* win = randy_window_create(ctx, &desc);
    assert(win != NULL);

    RandyWidgetId id = randy_checkbox_create(win, "enable feature", true);
    assert(id != 0);
    assert(randy_checkbox_get_checked(win, id) == true);

    randy_checkbox_set_checked(win, id, false);
    assert(randy_checkbox_get_checked(win, id) == false);

    int toggles = 0;
    randy_checkbox_set_callback(win, id, checkbox_cb, &toggles);
    assert(toggles == 0);
    /* callback wiring sanity (interactive toggling tested via examples) */
    randy_checkbox_set_checked(win, id, true);
    assert(randy_checkbox_get_checked(win, id) == true);

    randy_window_destroy(win);
    randy_shutdown(ctx);
}

TEST(null_safety) {
    /* None of these should crash */
    randy_window_destroy(NULL);
    randy_window_set_title(NULL, "x");
    randy_window_get_size(NULL, NULL, NULL);
    randy_render(NULL);
    randy_poll_events(NULL);

    assert(randy_label_create(NULL, "x")    == 0);
    assert(randy_button_create(NULL, "x")   == 0);
    assert(randy_checkbox_create(NULL, "x", false) == 0);
}

TEST(draw_commands_checkbox_exact) {
    drawspec_checkbox_exact();
}

TEST(draw_commands_radio_exact_contrast) {
    drawspec_radio_exact_contrast();
}

TEST(draw_commands_status_field_border) {
    drawspec_status_field_border();
}

TEST(draw_commands_sunken_panel_border) {
    drawspec_sunken_panel_border();
}

/* =========================================================================
 * Entry point
 * ========================================================================= */

int main(void) {
    printf("randy tests\n");
    printf("================\n");
    RUN(init_shutdown);
    RUN(init_shutdown_null_safe);
    RUN(result_strings);
    RUN(window_create_destroy);
    RUN(widget_label);
    RUN(widget_button_callback);
    RUN(widget_checkbox_state);
    RUN(null_safety);
    RUN(draw_commands_checkbox_exact);
    RUN(draw_commands_radio_exact_contrast);
    RUN(draw_commands_status_field_border);
    RUN(draw_commands_sunken_panel_border);
    printf("================\n");
    printf("%d / %d passed\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
