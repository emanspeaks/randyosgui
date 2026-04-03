#include <assert.h>
#include <stdio.h>
#include "randyosgui.h"

/* Minimal test harness — no external deps */

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

/* =========================================================================
 * Tests
 * ========================================================================= */

TEST(init_shutdown) {
    RandyosgContext* ctx = randyosgui_init();
    assert(ctx != NULL);
    randyosgui_shutdown(ctx);
}

TEST(init_shutdown_null_safe) {
    randyosgui_shutdown(NULL); /* must not crash */
}

TEST(result_strings) {
    assert(randyosgui_result_string(RANDYOSG_OK)                 != NULL);
    assert(randyosgui_result_string(RANDYOSG_ERR_NOMEM)          != NULL);
    assert(randyosgui_result_string(RANDYOSG_ERR_PLATFORM)       != NULL);
    assert(randyosgui_result_string(RANDYOSG_ERR_RENDERER)       != NULL);
    assert(randyosgui_result_string(RANDYOSG_ERR_INVALID_HANDLE) != NULL);
}

TEST(window_create_destroy) {
    RandyosgContext* ctx = randyosgui_init();
    assert(ctx != NULL);

    RandyosgWindowDesc desc = {
        .title     = "test window",
        .width     = 640,
        .height    = 480,
        .resizable = false,
        .decorated = true,
    };
    RandyosgWindow* win = randyosgui_window_create(ctx, &desc);
    assert(win != NULL);

    int w = 0, h = 0;
    randyosgui_window_get_size(win, &w, &h);
    assert(w == 640);
    assert(h == 480);

    assert(!randyosgui_window_should_close(win));

    randyosgui_window_destroy(win);
    randyosgui_shutdown(ctx);
}

TEST(widget_label) {
    RandyosgContext* ctx = randyosgui_init();
    RandyosgWindowDesc desc = { .title="t", .width=100, .height=100 };
    RandyosgWindow* win = randyosgui_window_create(ctx, &desc);
    assert(win != NULL);

    RandyosgWidgetId id = randyosgui_label_create(win, "hello");
    assert(id != 0);

    randyosgui_label_set_text(win, id, "updated");
    /* just checking it doesn't crash */

    randyosgui_window_destroy(win);
    randyosgui_shutdown(ctx);
}

static void button_cb(RandyosgWidgetId id, void* userdata) {
    (void)id;
    (*(int*)userdata)++;
}

TEST(widget_button_callback) {
    RandyosgContext* ctx = randyosgui_init();
    RandyosgWindowDesc desc = { .title="t", .width=100, .height=100 };
    RandyosgWindow* win = randyosgui_window_create(ctx, &desc);
    assert(win != NULL);

    RandyosgWidgetId id = randyosgui_button_create(win, "click me");
    assert(id != 0);

    int fired = 0;
    randyosgui_button_set_callback(win, id, button_cb, &fired);

    randyosgui_window_destroy(win);
    randyosgui_shutdown(ctx);
}

TEST(null_safety) {
    /* None of these should crash */
    randyosgui_window_destroy(NULL);
    randyosgui_window_set_title(NULL, "x");
    randyosgui_window_get_size(NULL, NULL, NULL);
    randyosgui_render(NULL);
    randyosgui_poll_events(NULL);

    assert(randyosgui_label_create(NULL, "x")    == 0);
    assert(randyosgui_button_create(NULL, "x")   == 0);
}

/* =========================================================================
 * Entry point
 * ========================================================================= */

int main(void) {
    printf("randyosgui tests\n");
    printf("================\n");
    RUN(init_shutdown);
    RUN(init_shutdown_null_safe);
    RUN(result_strings);
    RUN(window_create_destroy);
    RUN(widget_label);
    RUN(widget_button_callback);
    RUN(null_safety);
    printf("================\n");
    printf("%d / %d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
