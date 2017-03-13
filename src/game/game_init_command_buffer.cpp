#include <game/game.hpp>

void Game::init_command_buffer()
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = NULL;
    commandBufferAllocateInfo.commandPool = commandpool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    auto result = vkAllocateCommandBuffers
    (
        device,
        &commandBufferAllocateInfo,
        &commandbuffer
    );
    check(result == VK_SUCCESS);
}
