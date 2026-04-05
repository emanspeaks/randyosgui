#ifndef WIDGET_TOOLTIP_H
#define WIDGET_TOOLTIP_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_tooltip_create(RandyWindow* win, const char* text);

/* Renderer â€” draw */
void draw_tooltip(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent);

#endif /* WIDGET_TOOLTIP_H */
