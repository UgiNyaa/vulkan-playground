#include <game/game.hpp>

void Game::init_format()
{
    uint32_t formatCount;
    auto result = vkGetPhysicalDeviceSurfaceFormatsKHR
    (
        gpus[0],
        surface,
        &formatCount,
        NULL
    );
    check(result == VK_SUCCESS);
    check(formatCount > 0);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR
    (
        gpus[0],
        surface,
        &formatCount,
        surfaceFormats.data()
    );
    check(result == VK_SUCCESS);

    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
        format = VK_FORMAT_B8G8R8A8_UNORM;
    else
        format = surfaceFormats[0].format;
}
