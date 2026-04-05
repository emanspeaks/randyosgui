#include "slider.h"
#include "../renderer/renderer_private.h"
#include "../style.h"

RandyWidgetId randy_slider_create(RandyWindow* win,
                                           const char* label,
                                           int min_value,
                                           int max_value,
                                           int value) {
    if (!win) return 0;
    Widget* w = widget_alloc(win, WIDGET_SLIDER, label);
    if (!w) return 0;
    if (max_value < min_value) {
        int t = min_value;
        min_value = max_value;
        max_value = t;
    }
    w->min_value = min_value;
    w->max_value = max_value;
    w->value = clamp_int(value, min_value, max_value);
    return w->id;
}

void randy_slider_set_value(RandyWindow* win,
                                  RandyWidgetId id,
                                  int value) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_SLIDER) return;
    w->value = clamp_int(value, w->min_value, w->max_value);
}

int randy_slider_get_value(RandyWindow* win,
                                 RandyWidgetId id) {
    if (!win) return 0;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_SLIDER) return 0;
    return w->value;
}

void randy_slider_set_callback(RandyWindow* win,
                                     RandyWidgetId id,
                                     RandyValueCallback cb,
                                     void* userdata) {
    if (!win) return;
    Widget* w = widget_find(win, id);
    if (!w || w->kind != WIDGET_SLIDER) return;
    w->value_cb = cb;
    w->value_userdata = userdata;
}

void draw_slider(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent) {
    int track_y = w->y + (w->h / 2);
    int x0 = w->x + 12;
    int x1 = w->x + w->w - 12;
    if (x1 <= x0) x1 = x0 + 1;

    /* Sunken groove track */
    int track_h = 4;
    int ty = track_y - track_h / 2;
    draw_rect(cmd, extent, x0, ty, x1 - x0, track_h,
              g_style.input_background.r, g_style.input_background.g, g_style.input_background.b);
    draw_bevel(cmd, extent, x0, ty, x1 - x0, track_h, true);

    /* Filled portion in accent color */
    int knob_x = x0;
    if (w->max_value > w->min_value) {
        int range = w->max_value - w->min_value;
        knob_x = x0 + ((w->value - w->min_value) * (x1 - x0)) / range;
    }
    if (knob_x > x0 + 1) {
        draw_rect(cmd, extent, x0 + 1, ty + 1, knob_x - x0 - 1, track_h - 2,
                  g_style.highlight.r, g_style.highlight.g, g_style.highlight.b);
    }

    /* Raised thumb with 3D bevel */
    int knob_w = 12;
    int knob_h = 18;
    int ky = track_y - knob_h / 2;
    draw_rect(cmd, extent, knob_x - knob_w / 2, ky, knob_w, knob_h,
              g_style.button_face.r, g_style.button_face.g, g_style.button_face.b);
    draw_bevel(cmd, extent, knob_x - knob_w / 2, ky, knob_w, knob_h, w->pressed);

    Widget text_area = *w;
    text_area.y = w->y - 16;
    text_area.h = 16;
    draw_widget_text(r, cmd, &text_area, extent, 7,
                     g_style.text.r, g_style.text.g, g_style.text.b,
                     g_style.surface.r, g_style.surface.g, g_style.surface.b);
}
