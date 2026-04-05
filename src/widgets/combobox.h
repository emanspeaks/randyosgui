#ifndef WIDGET_COMBOBOX_H
#define WIDGET_COMBOBOX_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_combobox_create(RandyWindow* win, const char* selected_text);
void             randy_combobox_set_callback(RandyWindow* win, RandyWidgetId id,
                                                   RandyClickCallback cb, void* userdata);

/* Renderer â€” draw (same visual as dropdown for now) */
void draw_combobox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_COMBOBOX_H */
