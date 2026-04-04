#ifndef WIDGET_FIELD_BORDER_H
#define WIDGET_FIELD_BORDER_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_field_border_create(RandyosgWindow* win, const char* text,
                                                 bool disabled_style);

/* Renderer — draw */
void draw_field_border(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent);

#endif /* WIDGET_FIELD_BORDER_H */
