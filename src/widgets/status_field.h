#ifndef WIDGET_STATUS_FIELD_H
#define WIDGET_STATUS_FIELD_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_status_field_create(RandyWindow* win, const char* text);

/* Renderer â€” draw */
void draw_status_field(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent);

#endif /* WIDGET_STATUS_FIELD_H */
