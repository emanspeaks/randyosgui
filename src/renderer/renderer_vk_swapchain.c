#include "renderer_private.h"

/*
 * renderer_vk_swapchain.c — Vulkan swapchain, image views, render pass,
 *                           and framebuffer lifecycle.
 */

/* =========================================================================
 * Swapchain helpers
 * ========================================================================= */

static VkSurfaceFormatKHR choose_surface_format(VkPhysicalDevice dev,
                                                 VkSurfaceKHR surface) {
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count, NULL);
    VkSurfaceFormatKHR* formats = malloc(count * sizeof(VkSurfaceFormatKHR));
    VkSurfaceFormatKHR chosen = { VK_FORMAT_B8G8R8A8_SRGB,
                                   VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    if (!formats) return chosen;
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count, formats);
    if (count > 0) chosen = formats[0]; /* fallback to first */
    for (uint32_t i = 0; i < count; i++) {
        if (formats[i].format     == VK_FORMAT_B8G8R8A8_SRGB &&
            formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chosen = formats[i]; break;
        }
    }
    free(formats);
    return chosen;
}

static VkPresentModeKHR choose_present_mode(VkPhysicalDevice dev,
                                             VkSurfaceKHR surface) {
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count, NULL);
    VkPresentModeKHR* modes = malloc(count * sizeof(VkPresentModeKHR));
    VkPresentModeKHR chosen = VK_PRESENT_MODE_FIFO_KHR; /* guaranteed */
    if (!modes) return chosen;
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count, modes);
    for (uint32_t i = 0; i < count; i++) {
        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) { chosen = modes[i]; break; }
        if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) { chosen = modes[i]; }
    }
    free(modes);
    return chosen;
}

static VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR* caps,
                                PlatformWindow* win) {
    if (caps->currentExtent.width != UINT32_MAX) {
        return caps->currentExtent;
    }
    int w = 0, h = 0;
    platform_window_get_size(win, &w, &h);
    VkExtent2D ext = {
        .width  = (uint32_t)w,
        .height = (uint32_t)h,
    };
    ext.width  = ext.width  < caps->minImageExtent.width  ? caps->minImageExtent.width  : ext.width;
    ext.width  = ext.width  > caps->maxImageExtent.width  ? caps->maxImageExtent.width  : ext.width;
    ext.height = ext.height < caps->minImageExtent.height ? caps->minImageExtent.height : ext.height;
    ext.height = ext.height > caps->maxImageExtent.height ? caps->maxImageExtent.height : ext.height;
    return ext;
}

bool create_swapchain(RendererContext* r) {
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r->physical, r->surface, &caps);

    VkSurfaceFormatKHR fmt  = choose_surface_format(r->physical, r->surface);
    VkPresentModeKHR   mode = choose_present_mode(r->physical, r->surface);
    VkExtent2D         ext  = choose_extent(&caps, r->platform_win);

    uint32_t img_count = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && img_count > caps.maxImageCount)
        img_count = caps.maxImageCount;

    uint32_t families[2] = { r->graphics_family, r->present_family };
    bool same_family = (r->graphics_family == r->present_family);

    VkSwapchainCreateInfoKHR ci = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface               = r->surface,
        .minImageCount         = img_count,
        .imageFormat           = fmt.format,
        .imageColorSpace       = fmt.colorSpace,
        .imageExtent           = ext,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = same_family ? VK_SHARING_MODE_EXCLUSIVE
                                             : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = same_family ? 0 : 2,
        .pQueueFamilyIndices   = same_family ? NULL : families,
        .preTransform          = caps.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = mode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE,
    };

    if (vkCreateSwapchainKHR(r->device, &ci, NULL, &r->swapchain) != VK_SUCCESS) {
        fprintf(stderr, "[randy/renderer] vkCreateSwapchainKHR failed\n");
        return false;
    }

    r->swapchain_format = fmt.format;
    r->swapchain_extent = ext;

    vkGetSwapchainImagesKHR(r->device, r->swapchain, &r->image_count, NULL);
    r->images = malloc(r->image_count * sizeof(VkImage));
    if (!r->images) return false;
    vkGetSwapchainImagesKHR(r->device, r->swapchain, &r->image_count, r->images);
    return true;
}

bool create_image_views(RendererContext* r) {
    r->image_views = malloc(r->image_count * sizeof(VkImageView));
    if (!r->image_views) return false;
    for (uint32_t i = 0; i < r->image_count; i++) {
        VkImageViewCreateInfo ci = {
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image    = r->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = r->swapchain_format,
            .components = {
                VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0, .levelCount = 1,
                .baseArrayLayer = 0, .layerCount = 1,
            },
        };
        if (vkCreateImageView(r->device, &ci, NULL, &r->image_views[i]) != VK_SUCCESS) {
            fprintf(stderr, "[randy/renderer] vkCreateImageView[%u] failed\n", i);
            return false;
        }
    }
    return true;
}

/* =========================================================================
 * Render pass
 * ========================================================================= */

bool create_render_pass(RendererContext* r) {
    VkAttachmentDescription color_att = {
        .format         = r->swapchain_format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference color_ref = {
        .attachment = 0,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &color_ref,
    };

    VkSubpassDependency dep = {
        .srcSubpass    = VK_SUBPASS_EXTERNAL,
        .dstSubpass    = 0,
        .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    VkRenderPassCreateInfo ci = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1, .pAttachments = &color_att,
        .subpassCount    = 1, .pSubpasses   = &subpass,
        .dependencyCount = 1, .pDependencies = &dep,
    };

    if (vkCreateRenderPass(r->device, &ci, NULL, &r->render_pass) != VK_SUCCESS) {
        fprintf(stderr, "[randy/renderer] vkCreateRenderPass failed\n");
        return false;
    }
    return true;
}

/* =========================================================================
 * Framebuffers
 * ========================================================================= */

bool create_framebuffers(RendererContext* r) {
    r->framebuffers = malloc(r->image_count * sizeof(VkFramebuffer));
    if (!r->framebuffers) return false;
    for (uint32_t i = 0; i < r->image_count; i++) {
        VkFramebufferCreateInfo ci = {
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass      = r->render_pass,
            .attachmentCount = 1,
            .pAttachments    = &r->image_views[i],
            .width           = r->swapchain_extent.width,
            .height          = r->swapchain_extent.height,
            .layers          = 1,
        };
        if (vkCreateFramebuffer(r->device, &ci, NULL, &r->framebuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "[randy/renderer] vkCreateFramebuffer[%u] failed\n", i);
            return false;
        }
    }
    return true;
}

void destroy_swapchain_dependent(RendererContext* r) {
    if (!r || !r->device) return;

    if (r->framebuffers) {
        for (uint32_t i = 0; i < r->image_count; i++) {
            if (r->framebuffers[i]) vkDestroyFramebuffer(r->device, r->framebuffers[i], NULL);
        }
        free(r->framebuffers);
        r->framebuffers = NULL;
    }

    if (r->render_pass) {
        vkDestroyRenderPass(r->device, r->render_pass, NULL);
        r->render_pass = VK_NULL_HANDLE;
    }

    if (r->image_views) {
        for (uint32_t i = 0; i < r->image_count; i++) {
            if (r->image_views[i]) vkDestroyImageView(r->device, r->image_views[i], NULL);
        }
        free(r->image_views);
        r->image_views = NULL;
    }

    if (r->images) {
        free(r->images);
        r->images = NULL;
    }

    if (r->swapchain) {
        vkDestroySwapchainKHR(r->device, r->swapchain, NULL);
        r->swapchain = VK_NULL_HANDLE;
    }

    r->image_count = 0;
}

bool recreate_swapchain_dependent(RendererContext* r) {
    int fb_w = 0, fb_h = 0;
    platform_window_get_size(r->platform_win, &fb_w, &fb_h);
    if (fb_w <= 0 || fb_h <= 0) {
        return false;
    }

    vkDeviceWaitIdle(r->device);
    destroy_swapchain_dependent(r);

    if (!create_swapchain(r)) return false;
    if (!create_image_views(r)) return false;
    if (!create_render_pass(r)) return false;
    if (!create_framebuffers(r)) return false;
    return true;
}
