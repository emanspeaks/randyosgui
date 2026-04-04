#ifndef WIDGET_TREE_H
#define WIDGET_TREE_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_tree_item_create(RandyosgWindow* win, const char* label,
                                              int depth, bool expandable, bool expanded);

/* Renderer — draw (needs the widget list head to walk siblings/ancestors) */
void draw_tree_item(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent,
                    const Widget* widgets_head);

#endif /* WIDGET_TREE_H */
