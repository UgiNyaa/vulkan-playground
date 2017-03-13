#include <game/game.hpp>

void Game::init_memory_props()
{
    vkGetPhysicalDeviceMemoryProperties(gpus[0], &memoryProps);
}
