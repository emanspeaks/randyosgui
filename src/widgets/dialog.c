#include "dialog.h"

RandyWidgetId randy_dialog_create(RandyWindow* win, const char* title) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_DIALOG, title);
    if (!w) return 0;
    w->pref_w = 300;
    w->pref_h = 180;
    w->size_policy_h = SIZE_FIXED;
    w->size_policy_v = SIZE_FIXED;
    w->visible = false; /* dialogs start hidden */
    w->spacing = 6;
    w->padding = 8;
    return w->id;
}

void randy_dialog_show(RandyWindow* win, RandyWidgetId id) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_DIALOG) return;
    w->visible = true;
    win->needs_layout = true;
    win->needs_render = true;
}

void randy_dialog_hide(RandyWindow* win, RandyWidgetId id) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_DIALOG) return;
    w->visible = false;
    win->needs_layout = true;
    win->needs_render = true;
}

void draw_dialog(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent) {
    /* Dialog background */
    draw_rect(cmd, extent, w->x, w->y, w->w, w->h,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);

    /* Outer raised bevel */
    draw_bevel(cmd, extent, w->x, w->y, w->w, w->h, false);

    /* Title bar */
    int title_h = 18;
    draw_rect(cmd, extent, w->x + 3, w->y + 3, w->w - 6, title_h,
              g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);

    /* Title text */
    if (w->text) {
        Widget title_area;
        memset(&title_area, 0, sizeof(title_area));
        title_area.text = w->text;
        title_area.x = w->x + 6;
        title_area.y = w->y + 4;
        title_area.w = w->w - 12;
        title_area.h = title_h;
        title_area.visible = true;
        title_area.enabled = true;
        draw_widget_text(r, cmd, &title_area, extent, 2,
                         g_style.highlight_text.r, g_style.highlight_text.g, g_style.highlight_text.b,
                         g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);
    }

    /* Close button in title bar */
    int close_x = w->x + w->w - 18;
    int close_y = w->y + 5;
    draw_rect(cmd, extent, close_x, close_y, 14, 14,
              g_style.surface.r, g_style.surface.g, g_style.surface.b);
    draw_bevel(cmd, extent, close_x, close_y, 14, 14, false);
    /* X mark */
    draw_rect(cmd, extent, close_x + 4, close_y + 4, 2, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, close_x + 8, close_y + 4, 2, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, close_x + 5, close_y + 5, 1, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, close_x + 8, close_y + 5, 1, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, close_x + 6, close_y + 6, 2, 2,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, close_x + 5, close_y + 8, 1, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, close_x + 8, close_y + 8, 1, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, close_x + 4, close_y + 9, 2, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
    draw_rect(cmd, extent, close_x + 8, close_y + 9, 2, 1,
              g_style.text.r, g_style.text.g, g_style.text.b);
}
