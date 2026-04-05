#ifndef WIDGET_SEPARATOR_H
#define WIDGET_SEPARATOR_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_separator_create(RandyWindow* win);

/* Renderer â€” draw */
void draw_separator(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent);

#endif /* WIDGET_SEPARATOR_H */
