#include "renderer_private.h"

/*
 * renderer_vk.c — Vulkan instance / device / swapchain lifecycle.
 *
 * Covers:
 *   - Validation layer support check
 *   - Instance and debug messenger creation/destruction
 *   - Physical device selection and queue family enumeration
 *   - Logical device creation
 *   - Swapchain, image views, render pass, framebuffers (create + destroy)
 *   - Command pool / command buffers
 *   - Sync primitives (semaphores + fences)
 *   - renderer_create / renderer_destroy entry points
 */

/* =========================================================================
 * Validation / extension constants (only needed in this TU)
 * ========================================================================= */

static const char* const VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation",
};
static const uint32_t VALIDATION_LAYER_COUNT = 1;

static const char* const DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
static const uint32_t DEVICE_EXTENSION_COUNT = 1;

#ifdef NDEBUG
#  define ENABLE_VALIDATION 0
#else
#  define ENABLE_VALIDATION 1
#endif

/* =========================================================================
 * Debug messenger callback
 * ========================================================================= */

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
    VkDebugUtilsMessageTypeFlagsEXT             type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void*                                       userdata)
{
    (void)type; (void)userdata;
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        fprintf(stderr, "[vulkan] %s\n", data->pMessage);
    }
    return VK_FALSE;
}

/* =========================================================================
 * Validation layer support
 * ========================================================================= */

static bool check_validation_layer_support(void) {
    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, NULL);
    VkLayerProperties* props = malloc(count * sizeof(VkLayerProperties));
    if (!props) return false;
    vkEnumerateInstanceLayerProperties(&count, props);
    for (uint32_t i = 0; i < VALIDATION_LAYER_COUNT; i++) {
        bool found = false;
        for (uint32_t j = 0; j < count; j++) {
            if (strcmp(VALIDATION_LAYERS[i], props[j].layerName) == 0) {
                found = true; break;
            }
        }
        if (!found) { free(props); return false; }
    }
    free(props);
    return true;
}

/* =========================================================================
 * Instance creation
 * ========================================================================= */

static VkInstance create_instance(void) {
    VkApplicationInfo app = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "randy",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName        = "randy",
        .engineVersion      = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion         = VK_API_VERSION_1_2,
    };

    uint32_t glfw_ext_count = 0;
    const char** glfw_exts = platform_get_required_instance_extensions(&glfw_ext_count);

    uint32_t ext_count = glfw_ext_count;
    const char** exts = malloc((glfw_ext_count + 1) * sizeof(char*));
    if (!exts) return VK_NULL_HANDLE;
    for (uint32_t i = 0; i < glfw_ext_count; i++) exts[i] = glfw_exts[i];
    if (ENABLE_VALIDATION) {
        exts[ext_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }

    VkDebugUtilsMessengerCreateInfoEXT dbg_info = {
        .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
    };

    bool use_validation = ENABLE_VALIDATION && check_validation_layer_support();

    VkInstanceCreateInfo ci = {
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo        = &app,
        .enabledExtensionCount   = ext_count,
        .ppEnabledExtensionNames = exts,
        .enabledLayerCount       = use_validation ? VALIDATION_LAYER_COUNT : 0,
        .ppEnabledLayerNames     = use_validation ? VALIDATION_LAYERS : NULL,
        .pNext                   = use_validation ? (void*)&dbg_info : NULL,
    };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&ci, NULL, &instance);
    free(exts);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "[randy/renderer] vkCreateInstance failed: %d\n", result);
        return VK_NULL_HANDLE;
    }
    return instance;
}

/* =========================================================================
 * Debug messenger
 * ========================================================================= */

static VkDebugUtilsMessengerEXT create_debug_messenger(VkInstance instance) {
    PFN_vkCreateDebugUtilsMessengerEXT fn =
        (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (!fn) return VK_NULL_HANDLE;

    VkDebugUtilsMessengerCreateInfoEXT ci = {
        .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
    };
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    fn(instance, &ci, NULL, &messenger);
    return messenger;
}

static void destroy_debug_messenger(VkInstance instance,
                                    VkDebugUtilsMessengerEXT messenger) {
    if (messenger == VK_NULL_HANDLE) return;
    PFN_vkDestroyDebugUtilsMessengerEXT fn =
        (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (fn) fn(instance, messenger, NULL);
}

/* =========================================================================
 * Physical device + queue family selection
 * ========================================================================= */

static QueueFamilies find_queue_families(VkPhysicalDevice dev, VkSurfaceKHR surface) {
    QueueFamilies qf = { .found = false };
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, NULL);
    VkQueueFamilyProperties* props = malloc(count * sizeof(VkQueueFamilyProperties));
    if (!props) return qf;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props);

    bool has_graphics = false, has_present = false;
    for (uint32_t i = 0; i < count; i++) {
        if (!has_graphics && (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            qf.graphics = i; has_graphics = true;
        }
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &present_support);
        if (!has_present && present_support) {
            qf.present = i; has_present = true;
        }
        if (has_graphics && has_present) { qf.found = true; break; }
    }
    free(props);
    return qf;
}

static bool device_supports_extensions(VkPhysicalDevice dev) {
    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(dev, NULL, &count, NULL);
    VkExtensionProperties* props = malloc(count * sizeof(VkExtensionProperties));
    if (!props) return false;
    vkEnumerateDeviceExtensionProperties(dev, NULL, &count, props);
    for (uint32_t i = 0; i < DEVICE_EXTENSION_COUNT; i++) {
        bool found = false;
        for (uint32_t j = 0; j < count; j++) {
            if (strcmp(DEVICE_EXTENSIONS[i], props[j].extensionName) == 0) {
                found = true; break;
            }
        }
        if (!found) { free(props); return false; }
    }
    free(props);
    return true;
}

static bool swapchain_adequate(VkPhysicalDevice dev, VkSurfaceKHR surface) {
    uint32_t fmt_count = 0, mode_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &fmt_count, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &mode_count, NULL);
    return fmt_count > 0 && mode_count > 0;
}

static VkPhysicalDevice pick_physical_device(VkInstance instance,
                                              VkSurfaceKHR surface,
                                              QueueFamilies* out_qf) {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, NULL);
    if (count == 0) {
        fprintf(stderr, "[randy/renderer] no Vulkan-capable GPUs found\n");
        return VK_NULL_HANDLE;
    }
    VkPhysicalDevice* devs = malloc(count * sizeof(VkPhysicalDevice));
    if (!devs) return VK_NULL_HANDLE;
    vkEnumeratePhysicalDevices(instance, &count, devs);

    VkPhysicalDevice chosen = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < count; i++) {
        QueueFamilies qf = find_queue_families(devs[i], surface);
        if (!qf.found) continue;
        if (!device_supports_extensions(devs[i])) continue;
        if (!swapchain_adequate(devs[i], surface)) continue;
        chosen = devs[i];
        *out_qf = qf;
        break;
    }
    free(devs);
    if (chosen == VK_NULL_HANDLE) {
        fprintf(stderr, "[randy/renderer] no suitable physical device found\n");
    }
    return chosen;
}

/* =========================================================================
 * Logical device
 * ========================================================================= */

static VkDevice create_device(VkPhysicalDevice physical, QueueFamilies qf) {
    float priority = 1.0f;
    uint32_t families[2] = { qf.graphics, qf.present };
    uint32_t family_count = (qf.graphics == qf.present) ? 1 : 2;

    VkDeviceQueueCreateInfo queue_cis[2];
    for (uint32_t i = 0; i < family_count; i++) {
        queue_cis[i] = (VkDeviceQueueCreateInfo){
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = families[i],
            .queueCount       = 1,
            .pQueuePriorities = &priority,
        };
    }

    VkPhysicalDeviceFeatures features = {0};
    bool use_validation = ENABLE_VALIDATION && check_validation_layer_support();

    VkDeviceCreateInfo ci = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount    = family_count,
        .pQueueCreateInfos       = queue_cis,
        .enabledExtensionCount   = DEVICE_EXTENSION_COUNT,
        .ppEnabledExtensionNames = DEVICE_EXTENSIONS,
        .pEnabledFeatures        = &features,
        .enabledLayerCount       = use_validation ? VALIDATION_LAYER_COUNT : 0,
        .ppEnabledLayerNames     = use_validation ? VALIDATION_LAYERS : NULL,
    };

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vkCreateDevice(physical, &ci, NULL, &device);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "[randy/renderer] vkCreateDevice failed: %d\n", result);
        return VK_NULL_HANDLE;
    }
    return device;
}

/* Swapchain, image views, render pass, framebuffers are in renderer_vk_swapchain.c */

/* =========================================================================
 * Command pool / buffers
 * ========================================================================= */

static bool create_command_objects(RendererContext* r) {
    VkCommandPoolCreateInfo pool_ci = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = r->graphics_family,
    };
    if (vkCreateCommandPool(r->device, &pool_ci, NULL, &r->cmd_pool) != VK_SUCCESS) {
        fprintf(stderr, "[randy/renderer] vkCreateCommandPool failed\n");
        return false;
    }

    VkCommandBufferAllocateInfo alloc_info = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = r->cmd_pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
    };
    if (vkAllocateCommandBuffers(r->device, &alloc_info, r->cmd_bufs) != VK_SUCCESS) {
        fprintf(stderr, "[randy/renderer] vkAllocateCommandBuffers failed\n");
        return false;
    }
    return true;
}

/* =========================================================================
 * Sync objects
 * ========================================================================= */

static bool create_sync_objects(RendererContext* r) {
    VkSemaphoreCreateInfo sem_ci = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkFenceCreateInfo     fen_ci = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(r->device, &sem_ci, NULL, &r->img_available[i]) != VK_SUCCESS ||
            vkCreateSemaphore(r->device, &sem_ci, NULL, &r->render_done[i])   != VK_SUCCESS ||
            vkCreateFence    (r->device, &fen_ci, NULL, &r->in_flight[i])     != VK_SUCCESS) {
            fprintf(stderr, "[randy/renderer] sync object creation failed\n");
            return false;
        }
    }
    return true;
}

/* =========================================================================
 * renderer_create
 * ========================================================================= */

RendererContext* renderer_create(PlatformWindow* win) {
    RendererContext* r = (RendererContext*)randy_zalloc(sizeof(RendererContext));
    if (!r) return NULL;
    r->platform_win = win;

    /* 1. Instance */
    r->instance = create_instance();
    if (r->instance == VK_NULL_HANDLE) goto fail;

    /* 2. Debug messenger (debug builds only) */
    if (ENABLE_VALIDATION) {
        r->debug_messenger = create_debug_messenger(r->instance);
    }

    /* 3. Surface — delegated to platform layer */
    if (!platform_create_surface(win, r->instance, &r->surface)) goto fail;

    /* 4. Physical device */
    QueueFamilies qf;
    r->physical = pick_physical_device(r->instance, r->surface, &qf);
    if (r->physical == VK_NULL_HANDLE) goto fail;
    r->graphics_family = qf.graphics;
    r->present_family  = qf.present;

    /* 5. Logical device */
    r->device = create_device(r->physical, qf);
    if (r->device == VK_NULL_HANDLE) goto fail;

    vkGetDeviceQueue(r->device, r->graphics_family, 0, &r->graphics_queue);
    vkGetDeviceQueue(r->device, r->present_family,  0, &r->present_queue);

    /* 6. Swapchain */
    if (!create_swapchain(r))    goto fail;
    if (!create_image_views(r))  goto fail;

    /* 7. Render pass */
    if (!create_render_pass(r))  goto fail;

    /* 8. Framebuffers */
    if (!create_framebuffers(r)) goto fail;

    /* 9. Command objects */
    if (!create_command_objects(r)) goto fail;

    /* 10. Sync objects */
    if (!create_sync_objects(r)) goto fail;

    (void)init_text_system(r);

    return r;

fail:
    renderer_destroy(r);
    return NULL;
}

/* =========================================================================
 * renderer_destroy
 * ========================================================================= */

void renderer_destroy(RendererContext* r) {
    if (!r) return;

    shutdown_text_system(r);

    if (r->device) vkDeviceWaitIdle(r->device);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (r->img_available[i]) vkDestroySemaphore(r->device, r->img_available[i], NULL);
        if (r->render_done[i])   vkDestroySemaphore(r->device, r->render_done[i],   NULL);
        if (r->in_flight[i])     vkDestroyFence    (r->device, r->in_flight[i],     NULL);
    }

    if (r->cmd_pool) vkDestroyCommandPool(r->device, r->cmd_pool, NULL);

    destroy_swapchain_dependent(r);

    /* Surface destroyed by platform layer (it owns the GLFW window handle) */
    if (r->surface && r->instance) platform_destroy_surface(r->platform_win);

    if (r->device)   vkDestroyDevice(r->device, NULL);

    destroy_debug_messenger(r->instance, r->debug_messenger);

    if (r->instance) vkDestroyInstance(r->instance, NULL);

    free(r);
}
