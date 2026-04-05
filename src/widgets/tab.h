#ifndef WIDGET_TAB_H
#define WIDGET_TAB_H

#include "../randyosgui_internal.h"

/* Public API */
RandyWidgetId randy_tab_create(RandyWindow* win, const char* label, bool active);
void             randy_tab_set_active(RandyWindow* win, RandyWidgetId id,
                                           bool active);
bool             randy_tab_get_active(RandyWindow* win, RandyWidgetId id);
void             randy_tab_set_callback(RandyWindow* win, RandyWidgetId id,
                                             RandyClickCallback cb, void* userdata);

/* Renderer â€” draw (stateful: tracks the tab-strip underline across consecutive tabs) */
void draw_tab(RendererContext* r, VkCommandBuffer cmd,
              const Widget* w, VkExtent2D extent,
              const Widget* widgets_head, bool* in_tab_strip);

#endif /* WIDGET_TAB_H */
