#include <iostream>
#include <string>

#include <vulkan/vulkan.hpp>

int main(int argc, char const *argv[])
{
    VkResult result;

    // 1 - init intance
    VkApplicationInfo applicationInfo;
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = NULL;
    applicationInfo.pApplicationName = "Vulkan Playground";
    applicationInfo.applicationVersion = 0;
    applicationInfo.pEngineName = "Vulkan Playground";
    applicationInfo.engineVersion = 0;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = NULL;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = NULL;
    instanceCreateInfo.enabledExtensionCount = 0;
    instanceCreateInfo.ppEnabledExtensionNames = NULL;

    VkInstance instance;

    result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        std::cout << "cannot find a compatible Vulkan ICD" << '\n';
        exit(-1);
    }
    else if (result)
    {
        std::cout << "unknown error" << '\n';
        exit(-1);
    }

    std::cout << "1 - success!" << "\n\n";

    // 2 - enumerate devices
    uint32_t gpuCount = 1;
    result = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
    if (result)
    {
        std::cout << "2 - some errors occured" << '\n';
        exit(-1);
    } else if (gpuCount <= 0)
    {
        std::cout << "2 - no gpus" << '\n';
        exit(-1);
    }

    std::cout << "2 - " << gpuCount << " GPU(s) available" << '\n';

    std::vector<VkPhysicalDevice> gpus(gpuCount);
    result = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
    if (result)
    {
        std::cout << "2 - some errors occured" << '\n';
        exit(-1);
    }

    std::cout << "2 - GPU(s):" << '\n';
    for (size_t i = 0; i < gpuCount; i++)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(gpus[i], &physicalDeviceProperties);
        std::cout << "2 - " << i << ": "
         << physicalDeviceProperties.deviceName << '\n';
    }

    std::cout << "2 - success!" << "\n\n";

    // 3 - init device
    uint32_t queueFamilyPropertiesCount;
    vkGetPhysicalDeviceQueueFamilyProperties
    (
        gpus[0],
        &queueFamilyPropertiesCount,
        NULL
    );
    if (queueFamilyPropertiesCount <= 0)
    {
        std::cout << "3 - no queue family properties" << '\n';
        exit(-1);
    }

    std::cout << "3 - " << queueFamilyPropertiesCount
     << " queue family properties available" << '\n';

    std::vector<VkQueueFamilyProperties> queueFamilyProperties
    (
        queueFamilyPropertiesCount
    );
    vkGetPhysicalDeviceQueueFamilyProperties
    (
        gpus[0],
        &queueFamilyPropertiesCount,
        queueFamilyProperties.data()
    );

    std::cout << "3 - queue family properites:" << '\n';
    for (size_t i = 0; i < queueFamilyPropertiesCount; i++)
    {
        std::cout << "3 - " << i << ": Count: "
         << queueFamilyProperties[i].queueCount;

        auto flags = queueFamilyProperties[i].queueFlags;
        std::string flagstr = "";

        if
        (
            (flags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT
        )
            flagstr += "Graphic ";
        if
        (
            (flags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT
        )
            flagstr += "Compute ";
        if
        (
            (flags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT
        )
            flagstr += "Transfer ";
        if
        (
            (flags & VK_QUEUE_SPARSE_BINDING_BIT) == VK_QUEUE_SPARSE_BINDING_BIT
        )
            flagstr += "Sparse-Binding ";
        if ((int)flags == 0)
            flagstr += "No ";

        flagstr += "flag(s)";
        std::cout << "; " << flagstr << '\n';
    }

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;

    std::cout << "3 - get queue family with graphics bit" << '\n';
    bool found = false;
    for (size_t i = 0; i < queueFamilyPropertiesCount; i++)
    {
        auto flags = queueFamilyProperties[i].queueFlags;
        if ((flags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
        {
            deviceQueueCreateInfo.queueFamilyIndex = i;
            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << "there is no queue family with graphics bit" << '\n';
        exit(-1);
    }

    float queuePriorities[1] = {0.0};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = NULL;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = NULL;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = NULL;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = NULL;
    deviceCreateInfo.pEnabledFeatures = NULL;

    VkDevice device;
    result = vkCreateDevice(gpus[0], &deviceCreateInfo, NULL, &device);
    if (result)
    {
        std::cout << "errors occured" << '\n';
        exit(-1);
    }

    std::cout << "3 - success!" << "\n\n";

    // 4 - init command buffer
    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = NULL;
    commandPoolCreateInfo.queueFamilyIndex =
        deviceQueueCreateInfo.queueFamilyIndex;
    commandPoolCreateInfo.flags = 0;

    VkCommandPool commandPool;
    result = vkCreateCommandPool
    (
        device,
        &commandPoolCreateInfo,
        NULL,
        &commandPool
    );
    if (result)
    {
        std::cout << "some errors occured" << '\n';
        exit(-1);
    }

    std::cout << "4 - command pool created" << '\n';

    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = NULL;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    result = vkAllocateCommandBuffers
    (
        device,
        &commandBufferAllocateInfo,
        &commandBuffer
    );

    std::cout << "4 - command buffer(s) created" << '\n';

    std::cout << "4 - success" << '\n';

    // destroying
    VkCommandBuffer commandBuffers[] = { commandBuffer };
    vkFreeCommandBuffers(device, commandPool, 1, commandBuffers);

    vkDestroyCommandPool(device, commandPool, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);

    return 0;
}
