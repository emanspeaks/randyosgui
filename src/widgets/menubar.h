#ifndef WIDGET_MENUBAR_H
#define WIDGET_MENUBAR_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyWidgetId randy_menubar_create(RandyWindow* win);
RandyWidgetId randy_menu_item_create(RandyWindow* win, const char* label);
void             randy_menu_item_set_callback(RandyWindow* win, RandyWidgetId id,
                                                    RandyClickCallback cb, void* userdata);

/* Renderer â€” draw */
void draw_menubar(RendererContext* r, VkCommandBuffer cmd,
                  const Widget* w, VkExtent2D extent);
void draw_menu_item(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent);

#endif /* WIDGET_MENUBAR_H */
