#include <iostream>

#include <game/game.hpp>

void Game::init_uniform_buffer()
{
    auto projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    auto view = glm::lookAt(
        glm::vec3(-5, 3, -10), // Camera is at (-5,3,-10), in World Space
        glm::vec3(0, 0, 0),    // and looks at the origin
        glm::vec3(0, -1, 0)    // Head is up (set to 0,-1,0 to look upside-down)
        );
    auto model = glm::mat4(1.0f);
    // Vulkan clip space has inverted Y and half Z.
    auto clip = glm::mat4(1.0f,  0.0f, 0.0f, 0.0f,
                          0.0f, -1.0f, 0.0f, 0.0f,
                          0.0f,  0.0f, 0.5f, 0.0f,
                          0.0f,  0.0f, 0.5f, 1.0f);

    mvp = clip * projection * view * model;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = NULL;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.size = sizeof(mvp);
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = NULL;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    auto result = vkCreateBuffer(device, &bufferCreateInfo, NULL, &uniform.buf);
    check(result == VK_SUCCESS);

    VkMemoryRequirements memoryReqs;
    vkGetBufferMemoryRequirements(device, uniform.buf, &memoryReqs);

    VkMemoryAllocateInfo memoryAllocInfo;
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.pNext = NULL;
    memoryAllocInfo.memoryTypeIndex = 0;

    memoryAllocInfo.allocationSize = memoryReqs.size;
    auto pass = memory_type_from_properties
    (
        memoryReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &memoryAllocInfo.memoryTypeIndex
    );
    check(pass);

    result = vkAllocateMemory(device, &memoryAllocInfo, NULL, &uniform.mem);
    assert(result == VK_SUCCESS);

    uint8_t *pData;
    result = vkMapMemory
    (
        device,
        uniform.mem,
        0,
        memoryReqs.size,
        0,
        (void **)&pData
    );
    assert(result == VK_SUCCESS);

    memcpy(pData, &mvp, sizeof(mvp));

    vkUnmapMemory(device, uniform.mem);

    result = vkBindBufferMemory(device, uniform.buf, uniform.mem, 0);
    assert(result == VK_SUCCESS);

    uniform.bufferInfo.buffer = uniform.buf;
    uniform.bufferInfo.offset = 0;
    uniform.bufferInfo.range = sizeof(mvp);
}
