#include "label.h"

RandyWidgetId randy_label_create(RandyWindow* win, const char* text) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_LABEL, text);
    return w ? w->id : 0;
}

void randy_label_set_text(RandyWindow* win, RandyWidgetId id,
                                const char* text) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    free(w->text);
    w->text = randy_strdup(text);
    win->needs_layout = true;
}

void draw_label(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent) {
    draw_widget_rect(cmd, w, extent, g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
    draw_widget_text(r, cmd, w, extent, 6,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);
}
