#include "progress.h"

RandyWidgetId randy_progress_create(RandyWindow* win,
                                             const char* label,
                                             int max_value,
                                             int value) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_PROGRESS, label);
    if (!w) return 0;
    if (max_value < 1) max_value = 1;
    w->min_value = 0;
    w->max_value = max_value;
    w->value = clamp_int(value, 0, max_value);
    return w->id;
}

void randy_progress_set_value(RandyWindow* win,
                                    RandyWidgetId id,
                                    int value) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_PROGRESS) return;
    w->value = clamp_int(value, 0, w->max_value > 0 ? w->max_value : 1);
}

void draw_progress(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent) {
    (void)r;
    draw_widget_rect(cmd, w, extent, g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);

    int fill_w = 0;
    if (w->max_value > 0) {
        fill_w = ((w->w - 4) * w->value) / w->max_value;
    }
    if (fill_w > 0) {
        draw_rect(cmd, extent, w->x + 2, w->y + 2, fill_w, w->h - 4,
                  g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);
    }
}
