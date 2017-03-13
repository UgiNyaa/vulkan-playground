#include <game/game.hpp>

void Game::init_command_pool()
{
    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = NULL;
    commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    commandPoolCreateInfo.flags = 0;

    auto result = vkCreateCommandPool
    (
        device,
        &commandPoolCreateInfo,
        NULL,
        &commandpool
    );
    check(result == VK_SUCCESS);
}
