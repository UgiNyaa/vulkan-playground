#ifndef GAME_HPP
#define GAME_HPP

#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.hpp>

/* Number of samples needs to be the same at image creation,      */
/* renderpass creation and pipeline creation.                     */
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT
#define NUM_DESCRIPTOR_SETS 1

class Game
{
private:
    std::string appName;
    std::string engineName = "ugi_engine";

    // major variables
    VkInstance instance;
    VkDevice device;
    VkCommandPool commandpool;
    VkCommandBuffer commandbuffer;
    VkFormat format;
    VkPipelineLayout pipelineLayout;

    // major khr variables
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;

    std::vector<VkImageView> imageViews;
    std::vector<VkDescriptorSetLayout> descLayout;

    struct
    {
        VkFormat format;
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;

    struct
    {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } uniform;

    // enumeration results
    uint32_t queueFamilyCount;
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t presentingQueueFamilyIndex = UINT32_MAX;
    std::vector<VkPhysicalDevice> gpus;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    VkPhysicalDeviceMemoryProperties memoryProps;

    glm::mat4 mvp;

    // window options
    uint32_t width = 500;
    uint32_t height = 500;

    // xcb
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_intern_atom_reply_t *atomWmDeleteWindow;
    void init_window_connection_xcb();
    void init_window_xcb();

    void init_instance();
    void init_memory_props();
    void init_device();
    void init_command_pool();
    void init_command_buffer();
    void init_format();
    void init_depth_buffer();
    void init_uniform_buffer();
    void init_pipeline_layout();

    void init_surface();
    void init_swapchain();

    void enumerate_physical_devices();
    void enumerate_queue_family_properties();
    void init_graphics_presenting_queue();
    void init_image_views();

    void init_window_connection();
    void init_window();

    bool memory_type_from_properties
    (
        uint32_t typeBits,
        VkFlags requirements_mask,
        uint32_t *typeIndex
    );

    void check(bool shouldbe);

public:
    Game(std::string name);
    ~Game();
};

#endif /* end of include guard: GAME_HPP */