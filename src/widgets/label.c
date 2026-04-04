#include "label.h"

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
    win->needs_layout = true;
}

void draw_label(RendererContext* r, VkCommandBuffer cmd,
                const Widget* w, VkExtent2D extent) {
    draw_widget_rect(cmd, w, extent, WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);
    draw_widget_text(r, cmd, w, extent, 6,
                     WIN98.text_r, WIN98.text_g, WIN98.text_b,
                     WIN98.surface_r, WIN98.surface_g, WIN98.surface_b);
}
