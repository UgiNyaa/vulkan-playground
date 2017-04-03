#include <unistd.h>

#include <iostream>

#include <game/game.hpp>

void Game::draw_cube()
{
    uint32_t current_buffer;

    std::cout << "Semaphore" << '\n';
    VkSemaphore imageAcquiredSemaphore;
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = NULL;
    imageAcquiredSemaphoreCreateInfo.flags = 0;

    auto result = vkCreateSemaphore
    (
        device,
        &imageAcquiredSemaphoreCreateInfo,
        NULL,
        &imageAcquiredSemaphore
    );
    check(result == VK_SUCCESS);

    result = vkAcquireNextImageKHR
    (
        device,
        swapchain,
        UINT64_MAX,
        imageAcquiredSemaphore,
        VK_NULL_HANDLE,
        &current_buffer
    );
    check(result == VK_SUCCESS);

    std::cout << "begin render pass" << '\n';
    VkClearValue clear_values[2];
    clear_values[0].color.float32[0] = 0.2f;
    clear_values[0].color.float32[1] = 0.2f;
    clear_values[0].color.float32[2] = 0.2f;
    clear_values[0].color.float32[3] = 0.2f;
    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;

    VkRenderPassBeginInfo rp_begin;
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext = NULL;
    rp_begin.renderPass = renderPass;
    rp_begin.framebuffer = framebuffers[current_buffer];
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = width;
    rp_begin.renderArea.extent.height = height;
    rp_begin.clearValueCount = 2;
    rp_begin.pClearValues = clear_values;
    vkCmdBeginRenderPass
    (
        commandbuffer,
        &rp_begin,
        VK_SUBPASS_CONTENTS_INLINE
    );

    std::cout << "bind pipeline" << '\n';
    vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    std::cout << "bind desc sets" << '\n';
    vkCmdBindDescriptorSets
    (
        commandbuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        descSets.data(),
        0,
        NULL
    );

    std::cout << "bind vertex buffer" << '\n';
    const VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandbuffer, 0, 1, &vertexBuffer.buf, offsets);

    std::cout << "set viewport" << '\n';
    viewport.height = (float)height;
    viewport.width = (float)width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    viewport.x = 0;
    viewport.y = 0;
    vkCmdSetViewport(commandbuffer, 0, NUM_VIEWPORTS, &viewport);

    std::cout << "set scissor" << '\n';
    scissor.extent.height = height;
    scissor.extent.width = width;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(commandbuffer, 0 , NUM_SCISSORS, &scissor);

    std::cout << "draw" << '\n';
    vkCmdDraw(commandbuffer, 12*3, 1, 0, 0);

    std::cout << "end render pass" << '\n';
    vkCmdEndRenderPass(commandbuffer);

    std::cout << "end command buffer recording" << '\n';
    result = vkEndCommandBuffer(commandbuffer);
    check(result == VK_SUCCESS);

    std::cout << "create fence" << '\n';
    VkFenceCreateInfo fenceInfo;
    VkFence drawFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(device, &fenceInfo, NULL, &drawFence);

    std::cout << "submit command buffer" << '\n';
    const VkCommandBuffer cmd_bufs[] = { commandbuffer };
    VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submit_info[1] = {};
    submit_info[0].pNext = NULL;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].waitSemaphoreCount = 1;
    submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;
    submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;
    std::cout << graphicsQueue << '\n';
    result = vkQueueSubmit(graphicsQueue, 1, submit_info, drawFence);
    check(result == VK_SUCCESS);

    std::cout << "submit presentation" << '\n';
    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.swapchainCount = 1;
    present.pSwapchains = &swapchain;
    present.pImageIndices = &current_buffer;
    present.pWaitSemaphores = NULL;
    present.waitSemaphoreCount = 0;
    present.pResults = NULL;

    do {
        result = vkWaitForFences(device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);

    check(result == VK_SUCCESS);
    result = vkQueuePresentKHR(presentingQueue, &present);
    check(result == VK_SUCCESS);

    usleep((unsigned int)2000);

    std::cout << "destroying in draw_cube" << '\n';
    vkDestroySemaphore(device, imageAcquiredSemaphore, NULL);
    vkDestroyFence(device, drawFence, NULL);
}
