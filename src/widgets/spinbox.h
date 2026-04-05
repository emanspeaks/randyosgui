#ifndef WIDGET_SPINBOX_H
#define WIDGET_SPINBOX_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_spinbox_create(RandyWindow* win, int min_val, int max_val, int initial);
void             randy_spinbox_set_value(RandyWindow* win, RandyWidgetId id, int value);
int              randy_spinbox_get_value(RandyWindow* win, RandyWidgetId id);
void             randy_spinbox_set_callback(RandyWindow* win, RandyWidgetId id,
                                                  RandyValueCallback cb, void* userdata);

/* Renderer â€” draw */
void draw_spinbox(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent);

#endif /* WIDGET_SPINBOX_H */
