#ifndef WIDGET_TABLE_H
#define WIDGET_TABLE_H

#include "../randyosgui_internal.h"
#include "../renderer/renderer_private.h"

/* Public API */
RandyosgWidgetId randyosgui_table_header_create(RandyosgWindow* win, int num_cols,
                                                 const char* const* labels,
                                                 const int* col_widths);
RandyosgWidgetId randyosgui_table_row_create(RandyosgWindow* win, int num_cells,
                                              const char* const* cells, bool selected);
void             randyosgui_table_row_set_callback(RandyosgWindow* win, RandyosgWidgetId id,
                                                    RandyosgClickCallback cb, void* userdata);

/* Renderer — draw (row draw needs widget list head to find the matching header) */
void draw_table_header(RendererContext* r, VkCommandBuffer cmd,
                       const Widget* w, VkExtent2D extent);
void draw_table_row(RendererContext* r, VkCommandBuffer cmd,
                    const Widget* w, VkExtent2D extent,
                    const Widget* widgets_head);

#endif /* WIDGET_TABLE_H */
