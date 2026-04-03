#include <stdio.h>
#include "randyosgui.h"

static void on_button_click(RandyosgWidgetId id, void* userdata) {
    (void)id;
    int* count = (int*)userdata;
    (*count)++;
    printf("Button clicked! (count: %d)\n", *count);
}

int main(void) {
    RandyosgContext* ctx = randyosgui_init();
    if (!ctx) {
        fprintf(stderr, "randyosgui_init failed\n");
        return 1;
    }

    RandyosgWindowDesc desc = {
        .title     = "Hello randyosgui",
        .width     = 800,
        .height    = 600,
        .resizable = true,
        .decorated = true,
    };

    RandyosgWindow* win = randyosgui_window_create(ctx, &desc);
    if (!win) {
        fprintf(stderr, "randyosgui_window_create failed\n");
        randyosgui_shutdown(ctx);
        return 1;
    }

    RandyosgWidgetId label  = randyosgui_label_create(win, "Hello, world!");
    RandyosgWidgetId button = randyosgui_button_create(win, "Click me");

    int click_count = 0;
    randyosgui_button_set_callback(win, button, on_button_click, &click_count);

    (void)label; /* will be rendered automatically */

    while (!randyosgui_window_should_close(win)) {
        randyosgui_poll_events(ctx);
        randyosgui_render(win);
    }

    randyosgui_window_destroy(win);
    randyosgui_shutdown(ctx);
    return 0;
}
