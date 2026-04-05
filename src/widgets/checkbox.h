#ifndef WIDGET_CHECKBOX_H
#define WIDGET_CHECKBOX_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_checkbox_create(RandyWindow* win, const char* label,
                                             bool checked);
void             randy_checkbox_set_checked(RandyWindow* win, RandyWidgetId id,
                                                  bool checked);
bool             randy_checkbox_get_checked(RandyWindow* win, RandyWidgetId id);
void             randy_checkbox_set_callback(RandyWindow* win, RandyWidgetId id,
                                                   RandyToggleCallback cb, void* userdata);

/* Renderer â€” draw */
void draw_checkbox(RendererContext* r, VkCommandBuffer cmd,
                   const Widget* w, VkExtent2D extent);

#endif /* WIDGET_CHECKBOX_H */
