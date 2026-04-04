#include "textbox.h"

RandyosgWidgetId randyosgui_textbox_create(RandyosgWindow* win,
                                            const char* value,
                                            bool readonly) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TEXTBOX, value);
    if (!w) return 0;
    w->readonly = readonly;
    return w->id;
}

void randyosgui_textbox_set_value(RandyosgWindow* win,
                                   RandyosgWidgetId id,
                                   const char* value) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_TEXTBOX) return;
    free(w->text);
    w->text = randyosgui_strdup(value);
}

void draw_textbox(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent) {
    float bg = w->readonly ? WIN98.surface_r : 0.98f;
    float gg = w->readonly ? WIN98.surface_g : 0.98f;
    float bb = w->readonly ? WIN98.surface_b : 0.98f;
    draw_widget_rect(cmd, w, extent, bg, gg, bb);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
    draw_widget_text(r, cmd, w, extent, 4,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     bg, gg, bb);
}
