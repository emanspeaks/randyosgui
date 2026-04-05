#ifndef WIDGET_TOOLBAR_H
#define WIDGET_TOOLBAR_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_toolbar_create(RandyWindow* win);

/* Renderer â€” draw */
void draw_toolbar(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent);

#endif /* WIDGET_TOOLBAR_H */
