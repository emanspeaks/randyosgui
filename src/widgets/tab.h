#ifndef WIDGET_TAB_H
#define WIDGET_TAB_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_tab_create(RandyosgWindow* win, const char* label, bool active);
void             randyosgui_tab_set_active(RandyosgWindow* win, RandyosgWidgetId id,
                                           bool active);
bool             randyosgui_tab_get_active(RandyosgWindow* win, RandyosgWidgetId id);
void             randyosgui_tab_set_callback(RandyosgWindow* win, RandyosgWidgetId id,
                                             RandyosgClickCallback cb, void* userdata);

/* Renderer — draw (stateful: tracks the tab-strip underline across consecutive tabs) */
void draw_tab(RendererContext* r, VkCommandBuffer cmd,
              const Widget* w, VkExtent2D extent,
              const Widget* widgets_head, bool* in_tab_strip);

#endif /* WIDGET_TAB_H */
