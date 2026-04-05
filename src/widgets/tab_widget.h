#ifndef WIDGET_TAB_WIDGET_H
#define WIDGET_TAB_WIDGET_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API â€” compound tabbed container (tab bar + stacked pages) */
RandyWidgetId randy_tab_widget_create(RandyWindow* win);
RandyWidgetId randy_tab_widget_add_page(RandyWindow* win,
                                                  RandyWidgetId tab_widget_id,
                                                  const char* label);
void             randy_tab_widget_set_current(RandyWindow* win,
                                                    RandyWidgetId id,
                                                    int page_index);

/* Renderer â€” draw */
void draw_tab_widget(RendererContext* r, VkCommandBuffer cmd,
                     const Widget* w, VkExtent2D extent);

#endif /* WIDGET_TAB_WIDGET_H */
