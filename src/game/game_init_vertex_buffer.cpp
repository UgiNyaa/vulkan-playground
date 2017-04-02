#include <game/game.hpp>
#include <game/cube_data.h>

void Game::init_vertex_buffer()
{
    VkBufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = NULL;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.size = sizeof(g_vb_solid_face_colors_Data);
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = NULL;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    auto result = vkCreateBuffer
    (
        device,
        &bufferCreateInfo,
        NULL,
        &vertexBuffer.buf
    );
    check(result == VK_SUCCESS);

    VkMemoryRequirements memoryReqs;
    vkGetBufferMemoryRequirements(device, vertexBuffer.buf, &memoryReqs);

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

    result = vkAllocateMemory
    (
        device,
        &memoryAllocInfo,
        NULL, &vertexBuffer.mem
    );
    check(result == VK_SUCCESS);

    uint8_t *pData;
    result = vkMapMemory
    (
        device,
        vertexBuffer.mem,
        0,
        memoryReqs.size,
        0,
        (void **)&pData
    );
    check(result == VK_SUCCESS);

    memcpy
    (
        pData,
        g_vb_solid_face_colors_Data,
        sizeof(g_vb_solid_face_colors_Data)
    );

    vkUnmapMemory(device, vertexBuffer.mem);

    result = vkBindBufferMemory(device, vertexBuffer.buf, vertexBuffer.mem, 0);
    check(result == VK_SUCCESS);

    viBinding.binding = 0;
    viBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    viBinding.stride = sizeof(g_vb_solid_face_colors_Data[0]);

    viAttribs[0].binding = 0;
    viAttribs[0].location = 0;
    viAttribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    viAttribs[0].offset = 0;

    viAttribs[1].binding = 0;
    viAttribs[1].location = 1;
    viAttribs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    viAttribs[1].offset = 16;
}
