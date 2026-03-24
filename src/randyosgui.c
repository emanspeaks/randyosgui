#include "randyosgui_internal.h"

/* =========================================================================
 * Context
 * ========================================================================= */

RandyosgContext* randyosgui_init(void) {
    RandyosgContext* ctx = (RandyosgContext*)randyosgui_zalloc(sizeof(RandyosgContext));
    if (!ctx) return NULL;

    ctx->window_cap = 8;
    ctx->windows = (RandyosgWindow**)randyosgui_alloc(
        ctx->window_cap * sizeof(RandyosgWindow*));
    if (!ctx->windows) {
        free(ctx);
        return NULL;
    }
    return ctx;
}

void randyosgui_shutdown(RandyosgContext* ctx) {
    if (!ctx) return;
    for (uint32_t i = 0; i < ctx->window_count; i++) {
        randyosgui_window_destroy(ctx->windows[i]);
    }
    free(ctx->windows);
    free(ctx);
}

const char* randyosgui_result_string(RandyosgResult result) {
    switch (result) {
        case RANDYOSG_OK:                 return "OK";
        case RANDYOSG_ERR_NOMEM:          return "out of memory";
        case RANDYOSG_ERR_PLATFORM:       return "platform error";
        case RANDYOSG_ERR_RENDERER:       return "renderer error";
        case RANDYOSG_ERR_INVALID_HANDLE: return "invalid handle";
        default:                          return "unknown error";
    }
}

/* =========================================================================
 * Windows
 * ========================================================================= */

RandyosgWindow* randyosgui_window_create(RandyosgContext* ctx,
                                          const RandyosgWindowDesc* desc) {
    if (!ctx || !desc) return NULL;

    RandyosgWindow* win = (RandyosgWindow*)randyosgui_zalloc(sizeof(RandyosgWindow));
    if (!win) return NULL;

    win->ctx     = ctx;
    win->next_id = 1;

    win->platform = platform_window_create(desc);
    if (!win->platform) {
        free(win);
        return NULL;
    }

    win->renderer = renderer_create(win->platform);
    if (!win->renderer) {
        platform_window_destroy(win->platform);
        free(win);
        return NULL;
    }

    /* Register with context, growing array if needed */
    if (ctx->window_count == ctx->window_cap) {
        uint32_t new_cap = ctx->window_cap * 2;
        RandyosgWindow** new_arr = (RandyosgWindow**)realloc(
            ctx->windows, new_cap * sizeof(RandyosgWindow*));
        if (!new_arr) {
            renderer_destroy(win->renderer);
            platform_window_destroy(win->platform);
            free(win);
            return NULL;
        }
        ctx->windows  = new_arr;
        ctx->window_cap = new_cap;
    }
    ctx->windows[ctx->window_count++] = win;

    return win;
}

void randyosgui_window_destroy(RandyosgWindow* win) {
    if (!win) return;

    Widget* w = win->widgets;
    while (w) {
        Widget* next = w->next;
        free(w->text);
        free(w);
        w = next;
    }

    renderer_destroy(win->renderer);
    platform_window_destroy(win->platform);
    free(win);
}

bool randyosgui_window_should_close(RandyosgWindow* win) {
    if (!win) return true;
    return platform_window_should_close(win->platform);
}

void randyosgui_window_set_title(RandyosgWindow* win, const char* title) {
    if (!win) return;
    platform_window_set_title(win->platform, title);
}

void randyosgui_window_get_size(RandyosgWindow* win, int* width, int* height) {
    if (!win) return;
    platform_window_get_size(win->platform, width, height);
}

/* =========================================================================
 * Main loop helpers
 * ========================================================================= */

void randyosgui_poll_events(RandyosgContext* ctx) {
    (void)ctx;
    platform_poll_events();
}

void randyosgui_render(RandyosgWindow* win) {
    if (!win) return;
    renderer_render(win->renderer, win->widgets);
}

/* =========================================================================
 * Widgets — internal helpers
 * ========================================================================= */

static Widget* widget_alloc(RandyosgWindow* win, WidgetKind kind, const char* text) {
    Widget* w = (Widget*)randyosgui_zalloc(sizeof(Widget));
    if (!w) return NULL;

    w->id   = win->next_id++;
    w->kind = kind;
    w->text = randyosgui_strdup(text);

    /* Prepend to list */
    w->next      = win->widgets;
    win->widgets = w;

    return w;
}

static Widget* widget_find(RandyosgWindow* win, RandyosgWidgetId id) {
    for (Widget* w = win->widgets; w; w = w->next) {
        if (w->id == id) return w;
    }
    return NULL;
}

/* =========================================================================
 * Widgets — label
 * ========================================================================= */

RandyosgWidgetId randyosgui_label_create(RandyosgWindow* win, const char* text) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_LABEL, text);
    return w ? w->id : 0;
}

void randyosgui_label_set_text(RandyosgWindow* win, RandyosgWidgetId id,
                                const char* text) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    free(w->text);
    w->text = randyosgui_strdup(text);
}

/* =========================================================================
 * Widgets — button
 * ========================================================================= */

RandyosgWidgetId randyosgui_button_create(RandyosgWindow* win, const char* label) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_BUTTON, label);
    return w ? w->id : 0;
}

void randyosgui_button_set_callback(RandyosgWindow* win, RandyosgWidgetId id,
                                     RandyosgClickCallback cb, void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    w->click_cb       = cb;
    w->click_userdata = userdata;
}
