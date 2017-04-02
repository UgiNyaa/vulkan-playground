#include <game/game.hpp>

void Game::init_frame_buffers()
{
    VkImageView attachments[2];
    attachments[1] = depth.view;

    VkFramebufferCreateInfo frameBufferCreateInfo;
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = NULL;
    frameBufferCreateInfo.renderPass = renderPass;
    frameBufferCreateInfo.attachmentCount = 2;
    frameBufferCreateInfo.pAttachments = attachments;
    frameBufferCreateInfo.width = width;
    frameBufferCreateInfo.height = height;
    frameBufferCreateInfo.layers = 1;

    framebuffers.resize(swapchainBuffers.size());

    for (size_t i = 0; i < swapchainBuffers.size(); i++)
    {
        attachments[0] = swapchainBuffers[i].view;
        auto result = vkCreateFramebuffer
        (
            device,
            &frameBufferCreateInfo,
            NULL,
            &framebuffers[i]
        );
        check(result == VK_SUCCESS);
    }
}
