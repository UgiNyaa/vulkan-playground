#include <game/game.hpp>

void Game::enumerate_physical_devices()
{
    uint32_t gpuCount;
    auto result = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
    check(result == VK_SUCCESS);
    check(gpuCount > 0);

    gpus.resize(gpuCount);
    result = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
    check(result == VK_SUCCESS);
}

void Game::enumerate_queue_family_properties()
{
    vkGetPhysicalDeviceQueueFamilyProperties
    (
        gpus[0],
        &queueFamilyCount,
        NULL
    );
    check(queueFamilyCount > 0);

    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties
    (
        gpus[0],
        &queueFamilyCount,
        queueFamilyProperties.data()
    );
}
