#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct VideoDecode {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    uint32_t videoDecodeQueueIndex;
    VkDevice device;
    VkQueue videoDecodeQueue;
};

static bool creteInstance(struct VideoDecode *vd)
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
        return false;
    }

    vd->instance = instance;
    return true;
}

static bool pickPhysicalDevice(struct VideoDecode *vd)
{
    assert(vd->instance);

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
                vd->physicalDevice = device;
                vd->videoDecodeQueueIndex = q;
                return true;
            }
        }
    }

    printf("ERROR: Failed to find GPUs with VK_QUEUE_VIDEO_DECODE_BIT_KHR support\n");
    return false;
}

static bool createLogicalDevice(struct VideoDecode *vd)
{
    assert(vd->instance);
    assert(vd->physicalDevice);

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = vd->videoDecodeQueueIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = NULL;

    VkDevice device;
    VkResult result = vkCreateDevice(vd->physicalDevice, &createInfo, NULL, &device);
    if (result != VK_SUCCESS) {
        printf("ERROR: Failed to create logical device\n");
        return false;
    }

    vd->device = device;
    vkGetDeviceQueue(vd->device, vd->videoDecodeQueueIndex, 0, &vd->videoDecodeQueue);
    return true;
}

static bool initVideoDecode(struct VideoDecode *vd)
{
    printf("Creating Vulkan instance...\n");
    if (!creteInstance(vd)) {
        return false;
    }

    printf("Creating physical device...\n");
    if (!pickPhysicalDevice(vd)) {
        return false;
    }

    printf("Creating logical device...\n");
    if (!createLogicalDevice(vd)) {
        return false;
    }

    printf("VideoDecode struct initialized successfully :)\n");
    return true;
}

static void freeVideoDecode(struct VideoDecode *vd)
{
    vkDestroyDevice(vd->device, NULL);
    vkDestroyInstance(vd->instance, NULL);
}

int main(void)
{
    struct VideoDecode vd = {0};
    int ret = 0;

    printf("Initializing VideoDecode struct\n");
    if (!initVideoDecode(&vd)) {
        printf("ERROR: Error initializing VideoDecode struct\n");
        ret = -1;
    }

    printf("Destroying VideoDecode struct\n");
    freeVideoDecode(&vd);

    return ret;
}
