#ifndef WIDGET_STATUS_FIELD_H
#define WIDGET_STATUS_FIELD_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_status_field_create(RandyosgWindow* win, const char* text);

/* Renderer — draw */
void draw_status_field(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent);

#endif /* WIDGET_STATUS_FIELD_H */
