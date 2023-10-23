#include <stdbool.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct VideoDecode {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
};

static VkInstance creteInstance(void)
{
    VkInstance instance = NULL;

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Video Decode";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    const char *validationLayers[] = { "VK_LAYER_KHRONOS_validation" };

    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = NULL;
    createInfo.enabledLayerCount = ARRAY_SIZE(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;

    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
    if (result != VK_SUCCESS) {
        printf("ERROR: Failed to create VkInstance\n");
        return NULL;
    }

    return instance;
}

static VkPhysicalDevice pickPhysicalDevice(struct VideoDecode *vd)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vd->instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        printf("ERROR: Failed to find GPUs with Vulkan support\n");
        return NULL;
    }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(vd->instance, &deviceCount, devices);

    for (uint32_t d = 0; d < deviceCount; d++) {
        VkPhysicalDevice device = devices[d];

        // Get the device available queues
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

        VkQueueFamilyProperties queueFamilies[queueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

        for (uint32_t q = 0; q < queueFamilyCount; q++) {
            VkQueueFamilyProperties queueFamily = queueFamilies[q];
            if (queueFamily.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
                return device;
            }
        }
    }

    printf("ERROR: Failed to find GPUs with VK_QUEUE_VIDEO_DECODE_BIT_KHR support\n");
    return NULL;
}

static bool initVideoDecode(struct VideoDecode *vd)
{
    vd->instance = creteInstance();
    if (!vd->instance) {
        return false;
    }

    vd->physicalDevice = pickPhysicalDevice(vd);
    if (!vd->physicalDevice) {
        goto cleanupInstance;
    }

    return true;

cleanupInstance:
    vkDestroyInstance(vd->instance, NULL);
    return false;
}

static void freeVideoDecode(struct VideoDecode *vd)
{
    vkDestroyInstance(vd->instance, NULL);
}

int main(void)
{
    struct VideoDecode vd;

    printf("Initializing VideoDecode struct\n");
    if (!initVideoDecode(&vd)) {
        return -1;
    }

    printf("Destroying VideoDecode struct\n");
    freeVideoDecode(&vd);

    return 0;
}