#ifndef WIDGET_SLIDER_H
#define WIDGET_SLIDER_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_slider_create(RandyosgWindow* win, const char* label,
                                           int min_value, int max_value, int value);
void             randyosgui_slider_set_value(RandyosgWindow* win, RandyosgWidgetId id,
                                             int value);
int              randyosgui_slider_get_value(RandyosgWindow* win, RandyosgWidgetId id);
void             randyosgui_slider_set_callback(RandyosgWindow* win, RandyosgWidgetId id,
                                                RandyosgValueCallback cb, void* userdata);

/* Renderer — draw */
void draw_slider(RendererContext* r, VkCommandBuffer cmd,
                 const Widget* w, VkExtent2D extent);

#endif /* WIDGET_SLIDER_H */
