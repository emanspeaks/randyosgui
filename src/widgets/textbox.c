#include "textbox.h"

RandyWidgetId randy_textbox_create(RandyWindow* win,
                                            const char* value,
                                            bool readonly) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TEXTBOX, value);
    if (!w) return 0;
    w->readonly = readonly;
    return w->id;
}

void randy_textbox_set_value(RandyWindow* win,
                                   RandyWidgetId id,
                                   const char* value) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_TEXTBOX) return;
    free(w->text);
    w->text = randy_strdup(value);
}

void draw_textbox(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent) {
    float bg = w->readonly ? g_style.surface.r : g_style.input_background.r;
    float gg = w->readonly ? g_style.surface.g : g_style.input_background.g;
    float bb = w->readonly ? g_style.surface.b : g_style.input_background.b;
    draw_widget_rect(cmd, w, extent, bg, gg, bb);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
    draw_widget_text(r, cmd, w, extent, 4,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     bg, gg, bb);
}
