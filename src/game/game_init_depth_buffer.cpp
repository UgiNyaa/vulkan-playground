#include <iostream>

#include <game/game.hpp>

void Game::init_depth_buffer()
{
    // IMAGE CREATION
    VkImageCreateInfo imageCreateInfo;
    const VkFormat depthFormat = VK_FORMAT_D16_UNORM;

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(gpus[0], depthFormat, &props);

    if
    (
        props.linearTilingFeatures
      & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    )
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    else if
    (
        props.optimalTilingFeatures
      & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    )
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    else
    {
        std::cout << "VK_FORMAT_D16_UNORM unsupported" << '\n';
        exit(-1);
    }

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = NULL;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = depthFormat;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = NUM_SAMPLES;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = NULL;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.flags = 0;

    auto result = vkCreateImage(device, &imageCreateInfo, NULL, &depth.image);
    check(result == VK_SUCCESS);
    // IMAGE CREATED

    // MEMORY ALLOCATION
    VkMemoryRequirements memoryReqs;
    vkGetImageMemoryRequirements(device, depth.image, &memoryReqs);

    VkMemoryAllocateInfo memoryAllocInfo;
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.pNext = NULL;
    memoryAllocInfo.allocationSize = memoryReqs.size;
    auto pass = memory_type_from_properties
    (
        memoryReqs.memoryTypeBits,
        0,
        &memoryAllocInfo.memoryTypeIndex
    );
    check(pass);

    result = vkAllocateMemory(device, &memoryAllocInfo, NULL, &depth.mem);
    assert(result == VK_SUCCESS);

    result = vkBindImageMemory(device, depth.image, depth.mem, 0);
    assert(result == VK_SUCCESS);
    // MEMORY ALLOCATED

    // IMAGE VIEW CREATION
    VkImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext = NULL;
    imageViewCreateInfo.image = depth.image;
    imageViewCreateInfo.format = depthFormat;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.flags = 0;

    result = vkCreateImageView(device, &imageViewCreateInfo, NULL, &depth.view);
    check(result == VK_SUCCESS);
    // IMAGE VIEW CREATED

}
