#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_button_create(RandyWindow* win, const char* label);
void             randy_button_set_callback(RandyWindow* win, RandyWidgetId id,
                                                RandyClickCallback cb, void* userdata);

/* Renderer â€” draw */
void draw_button(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent);

#endif /* WIDGET_BUTTON_H */
