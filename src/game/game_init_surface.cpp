#include <game/game.hpp>

void Game::init_surface()
{
    // xcb
    VkXcbSurfaceCreateInfoKHR xcbSurfaceCreateInfo;
    xcbSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    xcbSurfaceCreateInfo.pNext = NULL;
    xcbSurfaceCreateInfo.connection = connection;
    xcbSurfaceCreateInfo.window = window;
    auto result = vkCreateXcbSurfaceKHR
    (
        instance,
        &xcbSurfaceCreateInfo,
        NULL,
        &surface
    );

    check(result == VK_SUCCESS);
}
