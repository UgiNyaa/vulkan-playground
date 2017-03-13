#include <game/game.hpp>

void Game::init_image_views()
{
    uint32_t swapchainImageCount;
    auto result = vkGetSwapchainImagesKHR
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

    imageViews.resize(swapchainImageCount);
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
        check(result == VK_SUCCESS);
    }
}
