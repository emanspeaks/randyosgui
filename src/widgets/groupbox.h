#ifndef WIDGET_GROUPBOX_H
#define WIDGET_GROUPBOX_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_groupbox_create(RandyosgWindow* win, const char* title);

/* Renderer — draw */
void draw_groupbox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_GROUPBOX_H */
