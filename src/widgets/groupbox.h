#ifndef WIDGET_GROUPBOX_H
#define WIDGET_GROUPBOX_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_groupbox_create(RandyWindow* win, const char* title);

/* Renderer â€” draw */
void draw_groupbox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_GROUPBOX_H */
