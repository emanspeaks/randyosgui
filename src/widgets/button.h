#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_button_create(RandyosgWindow* win, const char* label);
void             randyosgui_button_set_callback(RandyosgWindow* win, RandyosgWidgetId id,
                                                RandyosgClickCallback cb, void* userdata);

/* Renderer — draw */
void draw_button(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent);

#endif /* WIDGET_BUTTON_H */
