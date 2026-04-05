#ifndef WIDGET_SCROLL_AREA_H
#define WIDGET_SCROLL_AREA_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_scroll_area_create(RandyWindow* win);

/* Renderer â€” draw */
void draw_scroll_area(RendererContext* r, VkCommandBuffer cmd,
                      const Widget* w, VkExtent2D extent);

#endif /* WIDGET_SCROLL_AREA_H */
