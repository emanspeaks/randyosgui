#ifndef WIDGET_DROPDOWN_H
#define WIDGET_DROPDOWN_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_dropdown_create(RandyWindow* win, const char* value);
void             randy_dropdown_set_value(RandyWindow* win, RandyWidgetId id,
                                               const char* value);

/* Renderer â€” draw */
void draw_dropdown(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_DROPDOWN_H */
