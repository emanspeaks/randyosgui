#ifndef WIDGET_SLIDER_H
#define WIDGET_SLIDER_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_slider_create(RandyWindow* win, const char* label,
                                           int min_value, int max_value, int value);
void             randy_slider_set_value(RandyWindow* win, RandyWidgetId id,
                                             int value);
int              randy_slider_get_value(RandyWindow* win, RandyWidgetId id);
void             randy_slider_set_callback(RandyWindow* win, RandyWidgetId id,
                                                RandyValueCallback cb, void* userdata);

/* Renderer â€” draw */
void draw_slider(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent);

#endif /* WIDGET_SLIDER_H */
