#include "textedit.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_textedit_create(RandyWindow* win,
                                             const char* text,
                                             bool readonly) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_TEXTEDIT, text);
    if (!w) return 0;
    w->readonly = readonly;
    w->pref_h = 64;
    w->size_policy_v = SIZE_GROW;
    return w->id;
}

void randy_textedit_set_text(RandyWindow* win, RandyWidgetId id,
                                   const char* text) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w) return;
    free(w->text);
    w->text = randy_strdup(text);
    win->needs_render = true;
}

void draw_textedit(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent) {
    /* Qt Fusion: white bg with 1px border */
    float bg_r = w->readonly ? g_style.surface.r : g_style.input_background.r;
    float bg_g = w->readonly ? g_style.surface.g : g_style.input_background.g;
    float bg_b = w->readonly ? g_style.surface.b : g_style.input_background.b;

    draw_rect(cmd, extent, w->x, w->y, w->w, w->h, bg_r, bg_g, bg_b);
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, true);

    /* Draw text starting at top-left with padding */
    Widget text_area = *w;
    text_area.x = w->x + 4;
    text_area.y = w->y + 4;
    text_area.w = w->w - 8;
    text_area.h = w->h - 8;
    draw_widget_text(r, cmd, &text_area, extent, 2,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     bg_r, bg_g, bg_b);
}
