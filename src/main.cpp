#include <iostream>
#include <string>

//#include <xcb/xcb.h>

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.hpp>

int main(int argc, char const *argv[])
{
    VkResult result;

    // 1 - init intance
    VkApplicationInfo applicationInfo;
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = NULL;
    applicationInfo.pApplicationName = "Vulkan Playground";
    applicationInfo.applicationVersion = 0;
    applicationInfo.pEngineName = "Vulkan Playground";
    applicationInfo.engineVersion = 0;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    const char* instanceExtensionNames[] =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_XCB_SURFACE_EXTENSION_NAME
    };
    VkInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = NULL;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = NULL;
    instanceCreateInfo.enabledExtensionCount = 2;
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensionNames;

    VkInstance instance;

    result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        std::cout << "cannot find a compatible Vulkan ICD" << '\n';
        exit(-1);
    }
    else if (result)
    {
        std::cout << "unknown error" << '\n';
        exit(-1);
    }

    std::cout << "1 - success!" << "\n\n";

    // 2 - enumerate devices
    uint32_t gpuCount = 1;
    result = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
    if (result)
    {
        std::cout << "2 - some errors occured" << '\n';
        exit(-1);
    } else if (gpuCount <= 0)
    {
        std::cout << "2 - no gpus" << '\n';
        exit(-1);
    }

    std::cout << "2 - " << gpuCount << " GPU(s) available" << '\n';

    std::vector<VkPhysicalDevice> gpus(gpuCount);
    result = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
    if (result)
    {
        std::cout << "2 - some errors occured" << '\n';
        exit(-1);
    }

    std::cout << "2 - GPU(s):" << '\n';
    for (size_t i = 0; i < gpuCount; i++)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(gpus[i], &physicalDeviceProperties);
        std::cout << "2 - " << i << ": "
         << physicalDeviceProperties.deviceName << '\n';
    }

    std::cout << "2 - success!" << "\n\n";

    // 3 - init device
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties
    (
        gpus[0],
        &queueFamilyCount,
        NULL
    );
    if (queueFamilyCount <= 0)
    {
        std::cout << "3 - no queue family properties" << '\n';
        exit(-1);
    }

    std::cout << "3 - " << queueFamilyCount
     << " queue family properties available" << '\n';

    std::vector<VkQueueFamilyProperties> queueFamilyProperties
    (
        queueFamilyCount
    );
    vkGetPhysicalDeviceQueueFamilyProperties
    (
        gpus[0],
        &queueFamilyCount,
        queueFamilyProperties.data()
    );

    std::cout << "3 - queue family properites:" << '\n';
    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        std::cout << "3 - " << i << ": Count: "
         << queueFamilyProperties[i].queueCount;

        auto flags = queueFamilyProperties[i].queueFlags;
        std::string flagstr = "";

        if
        (
            (flags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT
        )
            flagstr += "Graphic ";
        if
        (
            (flags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT
        )
            flagstr += "Compute ";
        if
        (
            (flags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT
        )
            flagstr += "Transfer ";
        if
        (
            (flags & VK_QUEUE_SPARSE_BINDING_BIT) == VK_QUEUE_SPARSE_BINDING_BIT
        )
            flagstr += "Sparse-Binding ";
        if ((int)flags == 0)
            flagstr += "No ";

        flagstr += "flag(s)";
        std::cout << "; " << flagstr << '\n';
    }

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;

    std::cout << "3 - get queue family with graphics bit" << '\n';
    bool found = false;
    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        auto flags = queueFamilyProperties[i].queueFlags;
        if ((flags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
        {
            deviceQueueCreateInfo.queueFamilyIndex = i;
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << "there is no queue family with graphics bit" << '\n';
        exit(-1);
    }

    float queuePriorities[1] = {0.0};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = NULL;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

    const char* deviceExtensionNames[] =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = NULL;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 1;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensionNames;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = NULL;
    deviceCreateInfo.pEnabledFeatures = NULL;

    VkDevice device;
    result = vkCreateDevice(gpus[0], &deviceCreateInfo, NULL, &device);
    if (result)
    {
        std::cout << "errors occured" << '\n';
        exit(-1);
    }

    std::cout << "3 - success!" << "\n\n";

    // 4 - init command buffer
    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = NULL;
    commandPoolCreateInfo.queueFamilyIndex =
        deviceQueueCreateInfo.queueFamilyIndex;
    commandPoolCreateInfo.flags = 0;

    VkCommandPool commandPool;
    result = vkCreateCommandPool
    (
        device,
        &commandPoolCreateInfo,
        NULL,
        &commandPool
    );
    if (result)
    {
        std::cout << "some errors occured" << '\n';
        exit(-1);
    }

    std::cout << "4 - command pool created" << '\n';

    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = NULL;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    result = vkAllocateCommandBuffers
    (
        device,
        &commandBufferAllocateInfo,
        &commandBuffer
    );

    std::cout << "4 - command buffer(s) created" << '\n';

    std::cout << "4 - success" << "\n\n";

    // 5 - init swapchain
    VkSurfaceKHR surface;

    // TODO: init surface with apropriate platform (XLIB)
    // make connection
    std::cout << "5 - making xcb window connection" << '\n';
    uint32_t width = 500;
    uint32_t height = 500;
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    auto connection = xcb_connect(NULL, &scr);
    if (connection == NULL || xcb_connection_has_error(connection))
    {
        std::cout << "5 - unable to make an XCB connection" << '\n';
        exit(1);
    }

    setup = xcb_get_setup(connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0) xcb_screen_next(&iter);

    auto screen = iter.data;

    // make window
    std::cout << "5 - making window" << '\n';
    uint32_t value_mask, value_list[32];

    auto window = xcb_generate_id(connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window
    (
        connection,
        XCB_COPY_FROM_PARENT,
        window,
        screen->root,
        0, 0,
        width, height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        value_mask,
        value_list
    );

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom
    (
        connection,
        1,
        12,
        "WM_PROTOCOLS"
    );
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply
    (
        connection,
        cookie,
        0
    );

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom
    (
        connection,
        1,
        16,
        "WM_DELETE_WINDOW"
    );
    auto atom_wm_delete_window = xcb_intern_atom_reply(connection, cookie2, 0);

    xcb_change_property
    (
        connection,
        XCB_PROP_MODE_REPLACE,
        window,
        (*reply).atom,
        4,
        32,
        1,
        &(*atom_wm_delete_window).atom
    );

    free(reply);

    xcb_map_window(connection, window);

    const uint32_t coords[] = {100, 100};
    xcb_configure_window
    (
        connection,
        window,
        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
        coords
    );
    xcb_flush(connection);

    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(connection)))
        if ((e->response_type & ~0x80) == XCB_EXPOSE)
            break;

    std::cout << "5 - creating surface" << '\n';
    VkXcbSurfaceCreateInfoKHR xcbSurfaceCreateInfo;
    xcbSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    xcbSurfaceCreateInfo.pNext = NULL;
    xcbSurfaceCreateInfo.connection = connection;
    xcbSurfaceCreateInfo.window = window;
    result = vkCreateXcbSurfaceKHR
    (
        instance,
        &xcbSurfaceCreateInfo,
        NULL,
        &surface
    );
    if (result)
    {
        std::cout << "5 - errors occured" << '\n';
        std::cout << std::to_string((int)result) << '\n';
        exit(1);
    }

    // iterate over each queue family to learn
    // wheather it supports presenting
    std::cout << "5 - queue supports presenting:" << '\n';

    std::vector<VkBool32> supportsPresenting(queueFamilyCount);
    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR
        (
            gpus[0],
            i,
            surface,
            &supportsPresenting[i]
        );
        std::cout << "5 - " << i << ": "
         << (supportsPresenting[i] ? "true" : "false") << '\n';
    }

    // search for a graphics and a presenting queue family in the array
    // of queue families, try to find one which supports both
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t presentingQueueFamilyIndex = UINT32_MAX;
    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (graphicsQueueFamilyIndex == UINT32_MAX)
                graphicsQueueFamilyIndex = i;

            if (supportsPresenting[i] == VK_TRUE)
            {
                graphicsQueueFamilyIndex = i;
                presentingQueueFamilyIndex = i;
                break;
            }
        }
    }

    // check if it found a queue family that supports both
    if (presentingQueueFamilyIndex == UINT32_MAX)
    {
        // if it did not, then find a seperate persenting queue family
        for (size_t i = 0; i < queueFamilyCount; i++)
        {
            if (supportsPresenting[i] == VK_TRUE)
            {
                presentingQueueFamilyIndex = i;
                break;
            }
        }
    }
    supportsPresenting.clear();

    // generate error if could not find queues that support
    // graphics and present
    if
    (
        graphicsQueueFamilyIndex == UINT32_MAX
     || presentingQueueFamilyIndex == UINT32_MAX
    )
    {
        std::cout << "Could not find a queues for graphics present" << '\n';
        exit(-1);
    }

    if (deviceQueueCreateInfo.queueFamilyIndex != graphicsQueueFamilyIndex)
    {
        std::cout << "5 - old defined queue does not match new queue" << '\n';
        exit(1);
    }

    uint32_t formatCount;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR
    (
        gpus[0],
        surface,
        &formatCount,
        NULL
    );
    if (result)
    {
        std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
        exit(-1);
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR
    (
        gpus[0],
        surface,
        &formatCount,
        surfaceFormats.data()
    );
    if (result)
    {
        std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
        exit(-1);
    }

    VkFormat format;
    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
        format = VK_FORMAT_B8G8R8A8_UNORM;
    else
        format = surfaceFormats[0].format;
    surfaceFormats.clear();

    std::cout << "5 - create swapchain" << '\n';
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    (
        gpus[0],
        surface,
        &surfaceCapabilities
    );
    if (result)
    {
        std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
        exit(-1);
    }

    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR
    (
        gpus[0],
        surface,
        &presentModeCount,
        NULL
    );
    if (result)
    {
        std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
        exit(-1);
    }

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR
    (
        gpus[0],
        surface,
        &presentModeCount,
        presentModes.data()
    );
    if (result)
    {
        std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
        exit(-1);
    }

    VkExtent2D swapchainExtent;

    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        // if surface size is undefined, the size is set to
        // the size of the images requested
        swapchainExtent.width = width;
        swapchainExtent.height = height;

        if
        (
            swapchainExtent.width < surfaceCapabilities.minImageExtent.width
        )
            swapchainExtent.width = surfaceCapabilities.minImageExtent.width;
        else if
        (
            swapchainExtent.width > surfaceCapabilities.maxImageExtent.width
        )
            swapchainExtent.width = surfaceCapabilities.maxImageExtent.width;

        if
        (
            swapchainExtent.height < surfaceCapabilities.minImageExtent.height
        )
            swapchainExtent.height = surfaceCapabilities.minImageExtent.height;
        else if
        (
            swapchainExtent.height > surfaceCapabilities.maxImageExtent.height
        )
            swapchainExtent.height = surfaceCapabilities.maxImageExtent.height;
    }
    else
    {
        // if surface size is defined, the extent must match
        swapchainExtent = surfaceCapabilities.currentExtent;
    }

    // the FIFO present mode is guaranteed by the spec to be supported
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Determine the number of VkImage's to use in the swap chain.
    // We need to acquire only 1 presentable image at a time
    // Asking for minImageCount images ensures that we can acquire
    // 1 presentable image as long as we present it before attempting
    // to acquire another.
    uint32_t desiredNumberOfSwapchainImages = surfaceCapabilities.minImageCount;

    VkSurfaceTransformFlagBitsKHR preTransform;
    if
    (
        surfaceCapabilities.supportedTransforms
      & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
    )
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        preTransform = surfaceCapabilities.currentTransform;

    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = NULL;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
    swapchainCreateInfo.imageFormat = format;
    swapchainCreateInfo.imageExtent.width = swapchainExtent.width;
    swapchainCreateInfo.imageExtent.height = swapchainExtent.height;
    swapchainCreateInfo.preTransform = preTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.presentMode = swapchainPresentMode;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    swapchainCreateInfo.clipped = true;
    swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = NULL;

    uint32_t queueFamilyIndices[] =
    {
        (uint32_t)graphicsQueueFamilyIndex,
        (uint32_t)presentingQueueFamilyIndex
    };
    if (graphicsQueueFamilyIndex != presentingQueueFamilyIndex)
    {
        // If the graphics and present queues are from diffenrent queue
        // families, we eighter have to explicitly transfer ownership of images
        // between the queues, or we have to create the swapchain with
        // imageSharingMode as VK_SHARING_MODE_CONCURRENT
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    VkSwapchainKHR swapchain;
    result = vkCreateSwapchainKHR
    (
        device,
        &swapchainCreateInfo,
        NULL,
        &swapchain
    );
    if (result)
    {
        std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
        exit(-1);
    }

    uint32_t swapchainImageCount;
    result = vkGetSwapchainImagesKHR
    (
        device,
        swapchain,
        &swapchainImageCount,
        NULL
    );
    if (result)
    {
        std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
        exit(-1);
    }

    std::vector<VkImage> swapchainImages(swapchainImageCount);
    result = vkGetSwapchainImagesKHR
    (
        device,
        swapchain,
        &swapchainImageCount,
        swapchainImages.data()
    );
    if (result)
    {
        std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
        exit(-1);
    }

    std::vector<VkImageView> imageViews(swapchainImageCount);
    for (size_t i = 0; i < swapchainImageCount; i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = NULL;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        imageViewCreateInfo.subresourceRange.aspectMask =
            VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView
        (
            device,
            &imageViewCreateInfo,
            NULL,
            &imageViews[i]
        );
        if (result)
        {
            std::cout << "5 - errors occured (" << (int)result << ")" << '\n';
            exit(-1);
        }
    }

    std::cout << "5 - success!" << '\n';

    // destroying
    for (size_t i = 0; i < swapchainImageCount; i++)
        vkDestroyImageView(device, imageViews[i], NULL);

    vkDestroySurfaceKHR(instance, surface, NULL);
    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);

    VkCommandBuffer commandBuffers[] = { commandBuffer };
    vkFreeCommandBuffers(device, commandPool, 1, commandBuffers);

    vkDestroyCommandPool(device, commandPool, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);

    return 0;
}
