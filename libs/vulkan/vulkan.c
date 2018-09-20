#define HL_NAME(n) vulkan_##n

#include <hl.h>
#include <assert.h>

//hack
#define HL_VULKAN

#ifdef HL_VULKAN

#if defined(HL_IOS) || defined (HL_TVOS) || defined(HL_MAC)
//	Vulkan support provided by MoltenVK
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_syswm.h>
#	include <SDL2/SDL_vulkan.h>
#	include <vulkan/vulkan.h>
#elif defined(_WIN32)
#	include <SDL.h>
#	include <SDL_vulkan.h>
#	include <vulkan/vulkan.h>
#elif defined(HL_CONSOLE)
#	include <vulkan/vulkan.h>
#elif defined(HL_MESA)
#	error "No Vulkan support for HL_MESA"
#elif defined(HL_ANDROID)
#	include <SDL.h>
#	include <SDL_vulkan.h>
#	include <vulkan/vulkan.h>
#else
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_vulkan.h>
#	include <vulkan/vulkan.h>
#endif

#ifndef HL_VULKAN_APP_NAME
#define HL_VULKAN_APP_NAME "HashLink Vulkan App"
#endif

#ifndef HL_VULKAN_ENGINE_NAME
#define HL_VULKAN_ENGINE_NAME "HashLink Vulkan Engine"
#endif

#ifndef VK_ALLOC
#define VK_ALLOC hl_alloc_bytes
#endif

// Note : Might need to deal with vkGetInstanceProcAddr ?

static int VKLoadAPI() {
	// Load vulkan library
	SDL_Init(SDL_INIT_VIDEO); // Needed for Vulkan support
	SDL_ClearError();
	int result = SDL_Vulkan_LoadLibrary(NULL);
	if (result != 0) {
		printf("VKLoadAPI(): %s\n", SDL_GetError());
		return 1;
	}

	// Everything is okay
	return 0;
}

// helpers to store and marshall Vulkan structures

static vdynamic *alloc_i32(int v) {
	vdynamic *ret;
	ret = hl_alloc_dynamic(&hlt_i32);
	ret->v.i = v;
	return ret;
}

static vdynamic *alloc_ptr(void* ptr) {
	vdynamic *ret;
	ret = hl_alloc_dynamic(&hlt_dyn);
	ret->v.ptr = ptr;
	return ret;
}

#define ZIDX(val) ((val)?(val)->v.i:0)
#define ZPTR(val) ((val)?(val)->v.ptr:NULL)


// Used for validating return values of Vulkan API calls.
#define VK_CHECK_RESULT(f) 																	\
{																							\
	VkResult res = (f);																		\
	if (res != VK_SUCCESS)																	\
	{																						\
		hl_error("Fatal : VkResult is %d in %s at line %d\n", res,  __FILE__, __LINE__);	\
		assert(res == VK_SUCCESS);															\
	}																						\
}

// globals
HL_PRIM bool HL_NAME(vk_init)() {
	return VKLoadAPI() == 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Vulkan Types
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	hl_type *t;
	uint32_t apiVersion;
	uint32_t driverVersion;
	uint32_t vendorID;
	uint32_t deviceID;
	uint32_t deviceType;
	char* deviceName;
	char* pipelineCacheUUID;
} _VkPhysicalDeviceProperties;

typedef struct {
	hl_type *t;
	uint32_t	width;
	uint32_t	height;
	uint32_t	depth;
} _VkExtent3D;

typedef struct 
{
	hl_type *t;
	VkQueueFlags queueFlags;
	uint32_t queueCount;
	uint32_t timestampValidBits;
	_VkExtent3D* minImageTransferGranularity;
} _VkQueueFamilyProperties;


#define TSDLWINDOW			_ABSTRACT(sdl_window)
#define TVKINSTANCE			_ABSTRACT(vk_instance)
#define TVKPHYSICALDEVICE	_ABSTRACT(vk_physical_device)
#define TVKDEVICE			_ABSTRACT(vk_device)
#define TVKQUEUE			_ABSTRACT(vk_queue)
#define TVKPHYSICALDEVICEPROPERTIES _OBJ(_I32 _I32 _I32 _I32 _I32 _BYTES _BYTES)
#define TVKDEVICEQUEUEFAMILYPROPERTIES _OBJ(_I32 _I32 _I32 _OBJ(_I32 _I32 _I32))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Instances API
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// vkCreateInstance
HL_PRIM VkInstance* HL_NAME(vk_create_instance)(SDL_Window* window) {
	VkApplicationInfo appInfo;
	VkInstanceCreateInfo createInfo;
	unsigned int extensionCount = 0;
	VkExtensionProperties* extensions = NULL;
	VkInstance* instance = NULL;

	// Fill app info
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = HL_VULKAN_APP_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = HL_VULKAN_ENGINE_NAME;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Fill create info
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// TODO add support for validation layers
	createInfo.enabledLayerCount = 0;

	// Enumerate available extensions
	SDL_ClearError();
	if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL)) {
		printf("%s\n",SDL_GetError());
		//hl_error("Fatal : SDL_Vulkan_GetInstanceExtensions (1) error\n");
		return instance;
	}

	const char **extensionNames = malloc(sizeof(const char *) * extensionCount);
	if(!extensionNames) {
		hl_error("Fatal : Cannot allocate extensionNames\n");
		return instance;
	}

	if(!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames)) {
		hl_error("Fatal : SDL_Vulkan_GetInstanceExtensions (2) error\n");
		free(extensionNames);
		return instance;
	}

	// Now we can finish filling VkInstanceCreateInfo structure
	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensionNames;

	// TODO custom memory allocator ??
	// VkAllocationCallbacks allocator;

	instance = malloc(sizeof(VkInstance));
	VK_CHECK_RESULT(vkCreateInstance(&createInfo, NULL/*&allocator*/, instance));

	// Free memory
	free(extensionNames);

	// Return VkInstance object
	return instance;
}

HL_PRIM void HL_NAME(vk_destroy_instance)(VkInstance* pInstance) {
	if (pInstance != NULL) {
		vkDestroyInstance(*pInstance, NULL);
		free(pInstance); // TODO CHECK THAT (Not sure...)
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Physical devices API
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// vkEnumeratePhysicalDevices wrapper
static void vk_enumerate_physical_devices(VkInstance instance, VkPhysicalDevice** ppDevices, unsigned int *pDeviceCount) {

	// Retrieve number of physical devices
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, pDeviceCount, NULL));
	if (*pDeviceCount == 0) {
		hl_error("Fatal : vkEnumeratePhysicalDevices failed to find GPUs with Vulkan support!\n");
		return;
	}

	// Retrieve list of physical devices
	*ppDevices = malloc(sizeof(VkPhysicalDevice*) * (*pDeviceCount));
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, pDeviceCount, *ppDevices));
}

HL_PRIM VkPhysicalDevice* HL_NAME(vk_enumerate_physical_device_next)(VkInstance *pInstance)
{
	VkPhysicalDevice* result = NULL;
	VkInstance instance = *pInstance;
	static VkPhysicalDevice* pDevices = NULL;
	static unsigned int deviceCount = 0;
	static unsigned int deviceIndex = 0;

	if (deviceIndex > deviceCount)
		deviceIndex = 0;

	// Call inner enumerate at first call only
	if (deviceIndex == 0)
		vk_enumerate_physical_devices(instance, &pDevices, &deviceCount);

	// Iterate over found devices at each call
	if (deviceIndex >= deviceCount) {
		deviceIndex = 0;
	} else {
		result = &pDevices[deviceIndex++];
	}
	return result;
}

// vkGetPhysicalDeviceProperties
HL_PRIM void HL_NAME(vk_get_physical_device_properties)( VkPhysicalDevice *pPhysicalDevice, _VkPhysicalDeviceProperties *pProperties ) {

	// Call Vulkan API function
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(*pPhysicalDevice, &properties);

	pProperties->apiVersion = properties.apiVersion;
	pProperties->driverVersion = properties.driverVersion;
	pProperties->vendorID = properties.vendorID;
	pProperties->deviceID = properties.deviceID;
	pProperties->deviceType = properties.deviceType;
	pProperties->deviceName = (char*)malloc(strlen(properties.deviceName)+1);
	pProperties->pipelineCacheUUID = (char*)malloc(VK_UUID_SIZE);
	strcpy(pProperties->deviceName, properties.deviceName);
	memcpy((void*)pProperties->pipelineCacheUUID, properties.pipelineCacheUUID, VK_UUID_SIZE);
}

// vkGetPhysicalDeviceQueueFamilyProperties wrapper
static void vk_get_physical_device_queue_family_properties(VkPhysicalDevice device, unsigned int* pQueueFamilyPropertyCount, VkQueueFamilyProperties** ppQueueFamilyProperties)
{
	vkGetPhysicalDeviceQueueFamilyProperties(device, pQueueFamilyPropertyCount, NULL);
	*ppQueueFamilyProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * (*pQueueFamilyPropertyCount));
	vkGetPhysicalDeviceQueueFamilyProperties(device, pQueueFamilyPropertyCount, *ppQueueFamilyProperties);
}

HL_PRIM _VkQueueFamilyProperties *HL_NAME(vk_get_physical_device_queue_family_properties_next)( VkPhysicalDevice *pPhysicalDevice) {
	static int queueFamilyPropertiesIndex = 0;
	static unsigned int queueFamilyPropertiesCount = 0;
	static VkPhysicalDevice* pCurrentPhysicalDevice = NULL;
	static VkQueueFamilyProperties* pQueueFamilyProperties = NULL;
	_VkQueueFamilyProperties* p = NULL;

	// Retrieve data only if "new" device	
	if (pPhysicalDevice != pCurrentPhysicalDevice) {
		pCurrentPhysicalDevice = pPhysicalDevice;
		queueFamilyPropertiesIndex = 0;
		if (pQueueFamilyProperties != NULL)
			free(pQueueFamilyProperties);
		vk_get_physical_device_queue_family_properties(*pCurrentPhysicalDevice, &queueFamilyPropertiesCount, &pQueueFamilyProperties);
	}

	// Wraparound nicely, null terminated list
	if (queueFamilyPropertiesIndex >= queueFamilyPropertiesCount) {
		queueFamilyPropertiesIndex = 0;
		return NULL;
	}

	// Copy data
	if (queueFamilyPropertiesIndex < queueFamilyPropertiesCount) {
		p = (_VkQueueFamilyProperties*)malloc(sizeof(_VkQueueFamilyProperties));
		p->minImageTransferGranularity = (_VkExtent3D*)malloc(sizeof(_VkExtent3D));
		p->queueFlags = pQueueFamilyProperties[queueFamilyPropertiesIndex].queueFlags;
		p->queueCount = pQueueFamilyProperties[queueFamilyPropertiesIndex].queueCount;
		p->timestampValidBits = pQueueFamilyProperties[queueFamilyPropertiesIndex].timestampValidBits;
		p->minImageTransferGranularity->width = pQueueFamilyProperties[queueFamilyPropertiesIndex].minImageTransferGranularity.width;
		p->minImageTransferGranularity->height = pQueueFamilyProperties[queueFamilyPropertiesIndex].minImageTransferGranularity.height;
		p->minImageTransferGranularity->depth = pQueueFamilyProperties[queueFamilyPropertiesIndex].minImageTransferGranularity.depth;
		queueFamilyPropertiesIndex++;
	}

	return p;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Devices API
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HL_PRIM VkDevice *HL_NAME(vk_create_device)( VkPhysicalDevice *pPhysicalDevice, int iGfxQueueFamily, int nQueueCount ) {
    float qPriorities = 1.0f;
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.queueFamilyIndex = iGfxQueueFamily;
    deviceQueueCreateInfo.queueCount = nQueueCount;
    deviceQueueCreateInfo.pQueuePriorities = &qPriorities;
    
    /*const char* pDevExt[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };*/
    
 	VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.enabledExtensionCount = 0;//ARRAY_SIZE_IN_ELEMENTS(pDevExt);
    deviceCreateInfo.ppEnabledExtensionNames = NULL;//pDevExt;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

	VkDevice* device = malloc(sizeof(VkDevice));
	VkResult res = vkCreateDevice(*pPhysicalDevice, &deviceCreateInfo, NULL, device);
	return device;
}

HL_PRIM int HL_NAME(vk_device_wait_idle)( VkDevice *device ) {
	return vkDeviceWaitIdle(*device);
}

HL_PRIM void HL_NAME(vk_destroy_device)( VkDevice *device ) {
	vkDestroyDevice(*device, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Queues API
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HL_PRIM VkQueue *HL_NAME(vk_get_device_queue)( VkDevice *device, int queueFamilyIndex, int queueIndex) {
	VkQueue* queue = malloc(sizeof(VkQueue));
	vkGetDeviceQueue(*device, queueFamilyIndex, queueIndex, queue);
	return queue;
}

HL_PRIM int HL_NAME(vk_queue_wait_idle)( VkQueue *queue ) {
	return vkQueueWaitIdle(*queue);
}

// Command Buffers API

/*HL_PRIM vdynamic *HL_NAME(vk_create_command_pool)( vdynamic *device )
{
	return null;
}*/

DEFINE_PRIM(_BOOL, vk_init,_NO_ARG);
DEFINE_PRIM(TVKINSTANCE, vk_create_instance, TSDLWINDOW);
DEFINE_PRIM(_VOID, vk_destroy_instance, TVKINSTANCE);

DEFINE_PRIM(TVKPHYSICALDEVICE, vk_enumerate_physical_device_next, TVKINSTANCE);
DEFINE_PRIM(_VOID, vk_get_physical_device_properties, TVKPHYSICALDEVICE TVKPHYSICALDEVICEPROPERTIES)
DEFINE_PRIM(TVKDEVICEQUEUEFAMILYPROPERTIES, vk_get_physical_device_queue_family_properties_next, TVKPHYSICALDEVICE)

DEFINE_PRIM(TVKDEVICE, vk_create_device, TVKPHYSICALDEVICE _I32 _I32)
DEFINE_PRIM(_I32, vk_device_wait_idle, TVKDEVICE)
DEFINE_PRIM(_VOID, vk_destroy_device, TVKDEVICE)

DEFINE_PRIM(TVKQUEUE, vk_get_device_queue, TVKDEVICE _I32 _I32)
DEFINE_PRIM(_I32, vk_queue_wait_idle, TVKQUEUE)
#endif
