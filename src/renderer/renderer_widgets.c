#include "renderer_private.h"
#include "../widgets/widgets.h"

/*
 * renderer_widgets.c — widget dispatch loop, frame rendering, and test capture.
 *
 * Covers:
 *   - draw_widgets: per-kind dispatch to draw primitives (renderer_draw.c)
 *   - renderer_render: Vulkan frame acquire / record / submit / present
 *   - renderer_test_capture_widget_draw_ops: headless draw-op capture for tests
 */

/* =========================================================================
 * Widget dispatch
 * ========================================================================= */

static void draw_widgets(RendererContext* r, VkCommandBuffer cmd,
                         Widget* widgets, VkExtent2D extent) {
    bool in_tab_strip = false;

    for (Widget* w = widgets; w; w = w->next) {
        if (w->kind == WIDGET_LABEL)        { draw_label(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_BUTTON)       { draw_button(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_CHECKBOX)     { draw_checkbox(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_RADIO)        { draw_radio(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_TEXTBOX)      { draw_textbox(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_DROPDOWN)     { draw_dropdown(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_SLIDER)       { draw_slider(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_PROGRESS)     { draw_progress(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_GROUPBOX)     { draw_groupbox(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_TAB)          { draw_tab(r, cmd, w, extent, widgets, &in_tab_strip); continue; }
        if (w->kind == WIDGET_TREE_ITEM)    { draw_tree_item(r, cmd, w, extent, widgets); continue; }
        if (w->kind == WIDGET_TABLE_HEADER) { draw_table_header(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_TABLE_ROW)    { draw_table_row(r, cmd, w, extent, widgets); continue; }
        if (w->kind == WIDGET_FIELD_BORDER) { draw_field_border(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_STATUS_FIELD) { draw_status_field(r, cmd, w, extent); continue; }
        if (w->kind == WIDGET_SUNKEN_PANEL) { draw_sunken_panel(r, cmd, w, extent); continue; }

        in_tab_strip = false;
    }
}


/* =========================================================================
 * Frame rendering
 * ========================================================================= */

bool renderer_render(RendererContext* r, Widget* widgets) {
    int fb_w = 0, fb_h = 0;
    platform_window_get_size(r->platform_win, &fb_w, &fb_h);
    if (fb_w <= 0 || fb_h <= 0) {
        return false;
    }

    if ((uint32_t)fb_w != r->swapchain_extent.width ||
        (uint32_t)fb_h != r->swapchain_extent.height) {
        (void)recreate_swapchain_dependent(r);
        return false;
    }

    uint32_t frame = r->frame;

    vkWaitForFences(r->device, 1, &r->in_flight[frame], VK_TRUE, UINT64_MAX);

    uint32_t img_index;
    VkResult result = vkAcquireNextImageKHR(r->device, r->swapchain, UINT64_MAX,
                                             r->img_available[frame],
                                             VK_NULL_HANDLE, &img_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        (void)recreate_swapchain_dependent(r);
        return false;
    }
    if (result == VK_SUBOPTIMAL_KHR) {
        (void)recreate_swapchain_dependent(r);
        return false;
    }
    if (result != VK_SUCCESS) {
        return false;
    }

    vkResetFences(r->device, 1, &r->in_flight[frame]);

    VkCommandBuffer cmd = r->cmd_bufs[frame];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
    vkBeginCommandBuffer(cmd, &begin_info);

    VkClearValue clear_color = {{{ WIN98.surface_r, WIN98.surface_g, WIN98.surface_b, 1.0f }}};

    VkRenderPassBeginInfo rp_begin = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass      = r->render_pass,
        .framebuffer     = r->framebuffers[img_index],
        .renderArea      = { .offset = {0, 0}, .extent = r->swapchain_extent },
        .clearValueCount = 1,
        .pClearValues    = &clear_color,
    };
    vkCmdBeginRenderPass(cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    draw_window_chrome(cmd, r->swapchain_extent);
    draw_widgets(r, cmd, widgets, r->swapchain_extent);
    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &r->img_available[frame],
        .pWaitDstStageMask    = &wait_stage,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cmd,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &r->render_done[frame],
    };
    vkQueueSubmit(r->graphics_queue, 1, &submit, r->in_flight[frame]);

    VkPresentInfoKHR present = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &r->render_done[frame],
        .swapchainCount     = 1,
        .pSwapchains        = &r->swapchain,
        .pImageIndices      = &img_index,
    };
    VkResult present_result = vkQueuePresentKHR(r->present_queue, &present);
    if (present_result == VK_ERROR_OUT_OF_DATE_KHR ||
        present_result == VK_SUBOPTIMAL_KHR) {
        (void)recreate_swapchain_dependent(r);
        return false;
    }

    r->frame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
    return true;
}

/* =========================================================================
 * Headless test capture
 * ========================================================================= */

size_t renderer_test_capture_widget_draw_ops(const Widget* widget,
                                             uint32_t width,
                                             uint32_t height,
                                             RandyosgDrawOp* out_ops,
                                             size_t max_ops) {
    if (!widget || !out_ops || max_ops == 0 || width == 0 || height == 0) return 0;

    RendererContext fake = {0};
    VkExtent2D extent = { .width = width, .height = height };

    (void)init_text_system(&fake);

    g_capture.enabled = true;
    g_capture.ops = out_ops;
    g_capture.max_ops = max_ops;
    g_capture.count = 0;

    draw_widgets(&fake, VK_NULL_HANDLE, (Widget*)widget, extent);

    g_capture.enabled = false;
    shutdown_text_system(&fake);
    return (g_capture.count > max_ops) ? max_ops : g_capture.count;
}
