#ifndef WIDGET_CHECKBOX_H
#define WIDGET_CHECKBOX_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_checkbox_create(RandyosgWindow* win, const char* label,
                                             bool checked);
void             randyosgui_checkbox_set_checked(RandyosgWindow* win, RandyosgWidgetId id,
                                                  bool checked);
bool             randyosgui_checkbox_get_checked(RandyosgWindow* win, RandyosgWidgetId id);
void             randyosgui_checkbox_set_callback(RandyosgWindow* win, RandyosgWidgetId id,
                                                   RandyosgToggleCallback cb, void* userdata);

/* Renderer — draw */
void draw_checkbox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_CHECKBOX_H */
