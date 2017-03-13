#include <game/game.hpp>

void Game::init_descriptor_set()
{
    VkDescriptorPoolSize typeCount[1];
    typeCount[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    typeCount[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo descPoolCreateInfo;
    descPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolCreateInfo.pNext = NULL;
    descPoolCreateInfo.maxSets = 1;
    descPoolCreateInfo.poolSizeCount = 1;
    descPoolCreateInfo.pPoolSizes = typeCount;

    auto result = vkCreateDescriptorPool
    (
        device,
        &descPoolCreateInfo,
        NULL,
        &descPool
    );
    check(result == VK_SUCCESS);

    VkDescriptorSetAllocateInfo descSetAllocInfo;
    descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descSetAllocInfo.pNext = NULL;
    descSetAllocInfo.descriptorPool = descPool;
    descSetAllocInfo.descriptorSetCount = NUM_DESCRIPTOR_SETS;
    descSetAllocInfo.pSetLayouts = descLayout.data();

    descSets.resize(NUM_DESCRIPTOR_SETS);
    result = vkAllocateDescriptorSets
    (
        device,
        &descSetAllocInfo,
        descSets.data()
    );
    check(result == VK_SUCCESS);

    VkWriteDescriptorSet writes[1];
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].dstSet = descSets[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &uniform.bufferInfo;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    vkUpdateDescriptorSets(device, 1, writes, 0, NULL);
}
