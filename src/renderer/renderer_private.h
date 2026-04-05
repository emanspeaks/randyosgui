#ifndef RENDERER_PRIVATE_H
#define RENDERER_PRIVATE_H

/*
 * renderer_private.h â€” shared internals across renderer translation units.
 *
 * Included by renderer_vk.c, renderer_draw.c, and renderer_widgets.c.
 * Not part of the public or semi-public library interface.
 */

#include "../randyosgui_internal.h"
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

/* =========================================================================
 * Constants
 * ========================================================================= */

#define MAX_FRAMES_IN_FLIGHT 2

/* =========================================================================
 * DrawCapture â€” headless draw-op recording used by tests
 * ========================================================================= */

typedef struct {
    RandyDrawOp* ops;
    size_t          max_ops;
    size_t          count;
    bool            enabled;
} DrawCapture;

/* Defined in renderer_draw.c */
extern DrawCapture g_capture;

/* =========================================================================
 * Style / palette â€” all colors and metrics live in g_style (see style.h)
 * ========================================================================= */


/* =========================================================================
 * Queue family indices
 * ========================================================================= */

typedef struct { uint32_t graphics; uint32_t present; bool found; } QueueFamilies;

/* =========================================================================
 * RendererContext
 * ========================================================================= */

struct RendererContext {
    /* Core Vulkan objects */
    VkInstance               instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR             surface;       /* owned by platform layer */
    VkPhysicalDevice         physical;
    VkDevice                 device;

    /* Queues */
    uint32_t                 graphics_family;
    uint32_t                 present_family;
    VkQueue                  graphics_queue;
    VkQueue                  present_queue;

    /* Swapchain */
    VkSwapchainKHR           swapchain;
    VkFormat                 swapchain_format;
    VkExtent2D               swapchain_extent;
    uint32_t                 image_count;
    VkImage*                 images;        /* array[image_count] */
    VkImageView*             image_views;   /* array[image_count] */

    /* Render pass + framebuffers */
    VkRenderPass             render_pass;
    VkFramebuffer*           framebuffers;  /* array[image_count] */

    /* Command pool / buffers */
    VkCommandPool            cmd_pool;
    VkCommandBuffer          cmd_bufs[MAX_FRAMES_IN_FLIGHT];

    /* Sync objects */
    VkSemaphore              img_available[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore              render_done[MAX_FRAMES_IN_FLIGHT];
    VkFence                  in_flight[MAX_FRAMES_IN_FLIGHT];

    uint32_t                 frame;         /* current frame index mod MAX_FRAMES_IN_FLIGHT */

    /* Back-reference to platform window (for surface + resize queries) */
    PlatformWindow*          platform_win;

    FT_Library               ft_library;
    FT_Face                  font_sans;
    FT_Face                  font_mono;
};

/* =========================================================================
 * Cross-file internal function declarations
 *
 * Functions whose implementations live in one renderer_*.c file but are
 * called from another.  All are internal-only; none appear in the public
 * or semi-public (randyosgui_internal.h) headers.
 * ========================================================================= */

/* renderer_vk_swapchain.c â†’ called from renderer_vk.c and renderer_widgets.c */
bool create_swapchain(RendererContext* r);
bool create_image_views(RendererContext* r);
bool create_render_pass(RendererContext* r);
bool create_framebuffers(RendererContext* r);
void destroy_swapchain_dependent(RendererContext* r);
bool recreate_swapchain_dependent(RendererContext* r);

/* renderer_draw.c â†’ called from renderer_vk.c and renderer_widgets.c */
bool init_text_system(RendererContext* r);
void shutdown_text_system(RendererContext* r);

/* Draw primitives â€” renderer_draw.c â†’ called from renderer_widgets.c */
void draw_rect(VkCommandBuffer cmd, VkExtent2D extent,
               int x, int y, int rw, int rh,
               float r, float g, float b);
void draw_bevel(VkCommandBuffer cmd, VkExtent2D extent,
                int x, int y, int w, int h, bool sunken);
void draw_window_chrome(VkCommandBuffer cmd, VkExtent2D extent);
void draw_widget_rect(VkCommandBuffer cmd, const Widget* w, VkExtent2D extent,
                      float r, float g, float b);
void draw_text_span(RendererContext* r, VkCommandBuffer cmd, VkExtent2D extent,
                    const Widget* w, FT_Face face,
                    const char* begin, const char* end,
                    int pen_x, int baseline_y,
                    float tr, float tg, float tb,
                    float br, float bg, float bb);
void draw_widget_text(RendererContext* r, VkCommandBuffer cmd, const Widget* w,
                      VkExtent2D extent, int text_offset_y,
                      float tr, float tg, float tb,
                      float br, float bg, float bb);
int  approx_text_px(const char* s);

/* Tree helpers â€” renderer_draw.c â†’ called from renderer_widgets.c */
bool          tree_has_next_sibling(const Widget* row);
bool          tree_has_prev_sibling(const Widget* block_start, const Widget* row);
const Widget* tree_find_ancestor(const Widget* block_start, const Widget* row,
                                  int ancestor_level);

#endif /* RENDERER_PRIVATE_H */
