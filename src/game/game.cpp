#include <iostream>

#include <game/game.hpp>

Game::Game(std::string name)
{
    appName = name;

    std::cout << "initializing instance" << '\n';
    init_instance();

    std::cout << "enumerating devices" << '\n';
    enumerate_physical_devices();
    std::cout << "enumerating queue family properties" << '\n';
    enumerate_queue_family_properties();

    std::cout << "initializing memory properties" << '\n';
    init_memory_props();

    std::cout << "initializing window connection" << '\n';
    init_window_connection();
    std::cout << "initializing window" << '\n';
    init_window();
    std::cout << "initializing surface" << '\n';
    init_surface();

    std::cout << "initializing graphics and presenting queue" << '\n';
    init_graphics_presenting_queue();
    std::cout << "initializing device" << '\n';
    init_device();

    std::cout << "initializing depth buffer" << '\n';
    init_depth_buffer();

    std::cout << "initializing format" << '\n';
    init_format();
    std::cout << "initializing swapchain" << '\n';
    init_swapchain();

    std::cout << "initializing command pool" << '\n';
    init_command_pool();
    std::cout << "initializing command buffer" << '\n';
    init_command_buffer();

    std::cout << "initializing uniform buffer" << '\n';
    init_uniform_buffer();

    std::cout << "initializing pipeline layout" << '\n';
    init_pipeline_layout();
    std::cout << "initializing descriptor set" << '\n';
    init_descriptor_set();

    std::cout << "initializing render pass" << '\n';
    init_render_pass();

    std::cout << "initializing shaders" << '\n';
    init_shaders();

    std::cout << "initializing framebuffers" << '\n';
    init_frame_buffers();

    std::cout << "initializing vertex buffer" << '\n';
    init_vertex_buffer();

    std::cout << "initializing pipeline" << '\n';
    init_pipeline();

    std::cout << "draw cube" << '\n';
    draw_cube();
}

Game::~Game()
{
    std::cout << "destorying" << '\n';

    vkDestroyPipeline(device, pipeline, NULL);

    vkDestroyShaderModule(device, shaderStages[0].module, NULL);
    vkDestroyShaderModule(device, shaderStages[1].module, NULL);

    vkDestroyDescriptorPool(device, descPool, NULL);

    for (size_t i = 0; i < swapchainBuffers.size(); i++)
    {
        vkDestroyImageView(device, swapchainBuffers[i].view, NULL);
        vkDestroyImage(device, swapchainBuffers[i].image, NULL);
    }

    vkDestroyBuffer(device, uniform.buf, NULL);
    vkFreeMemory(device, uniform.mem, NULL);

    vkDestroyBuffer(device, vertexBuffer.buf, NULL);
    vkFreeMemory(device, vertexBuffer.mem, NULL);

    vkDestroyImageView(device, depth.view, NULL);
    vkDestroyImage(device, depth.image, NULL);
    vkFreeMemory(device, depth.mem, NULL);

    vkDestroySurfaceKHR(instance, surface, NULL);
    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);

    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
}

bool Game::memory_type_from_properties
(
    uint32_t typeBits,
    VkFlags requirements_mask,
    uint32_t *typeIndex
)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < memoryProps.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            // Type is available, does it match user properties?
            if
            (
                (memoryProps.memoryTypes[i].propertyFlags & requirements_mask)
             == requirements_mask
            )
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

void Game::check(bool shouldbe)
{
    if (!shouldbe)
    {
        std::cout << "check failed" << '\n';
        exit(-1);
    }
}
