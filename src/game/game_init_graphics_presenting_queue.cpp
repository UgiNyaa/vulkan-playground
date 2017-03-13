#include <game/game.hpp>

void Game::init_graphics_presenting_queue()
{
    std::vector<VkBool32> supportsPresentingQueues(queueFamilyCount);
    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR
        (
            gpus[0],
            i,
            surface,
            &supportsPresentingQueues[i]
        );
    }

    // search for a graphics and a presenting queue family in the array
    // of queue families, try to find one which supports both
    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (graphicsQueueFamilyIndex == UINT32_MAX)
                graphicsQueueFamilyIndex = i;

            if (supportsPresentingQueues[i] == VK_TRUE)
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
            if (supportsPresentingQueues[i] == VK_TRUE)
            {
                presentingQueueFamilyIndex = i;
                break;
            }
        }
    }
    supportsPresentingQueues.clear();

    check(graphicsQueueFamilyIndex != UINT32_MAX);
    check(presentingQueueFamilyIndex != UINT32_MAX);
}
