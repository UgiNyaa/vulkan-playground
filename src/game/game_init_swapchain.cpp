#include <game/game.hpp>

void Game::init_swapchain()
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    (
        gpus[0],
        surface,
        &surfaceCapabilities
    );
    check(result == VK_SUCCESS);

    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR
    (
        gpus[0],
        surface,
        &presentModeCount,
        NULL
    );
    check(result == VK_SUCCESS);
    check(presentModeCount > 0);

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR
    (
        gpus[0],
        surface,
        &presentModeCount,
        presentModes.data()
    );
    check(result == VK_SUCCESS);

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

    result = vkCreateSwapchainKHR
    (
        device,
        &swapchainCreateInfo,
        NULL,
        &swapchain
    );
    check(result == VK_SUCCESS);

    uint32_t swapchainImageCount;
    result = vkGetSwapchainImagesKHR
    (
        device,
        swapchain,
        &swapchainImageCount,
        NULL
    );
    check(result == VK_SUCCESS);
    check(swapchainImageCount > 0);

    std::vector<VkImage> swapchainImages(swapchainImageCount);
    result = vkGetSwapchainImagesKHR
    (
        device,
        swapchain,
        &swapchainImageCount,
        swapchainImages.data()
    );
    check(result == VK_SUCCESS);

    swapchainBuffers.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        swapchainBuffers[i].image = swapchainImages[i];
    }

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
            &swapchainBuffers[i].view
        );
        check(result == VK_SUCCESS);
    }
}
