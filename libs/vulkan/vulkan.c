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


#define TSDLWINDOW					_ABSTRACT(sdl_window)
#define TVKINSTANCE					_ABSTRACT(vk_instance)
#define TVKPHYSICALDEVICE			_ABSTRACT(vk_physical_device)
#define TVKDEVICE					_ABSTRACT(vk_device)
#define TVKQUEUE					_ABSTRACT(vk_queue)
#define TVKSURFACEKHR				_ABSTRACT(vk_surface_khr)
#define TVKSURFACEFORMATKHR			_ABSTRACT(vk_surface_format_khr)
#define TVKSURFACECAPABILITIESKHR	_ABSTRACT(vk_surface_capabilities_khr)
#define TVKPRESENTMODEKHR			_ABSTRACT(vk_present_mode_khr)
#define TVKSWAPCHAINKHR				_ABSTRACT(vk_swapchain_khr)
#define TVKIMAGE					_ABSTRACT(vk_image)
#define TVKIMAGEVIEW				_ABSTRACT(vk_image_view)
#define TVKSHADERMODULE				_ABSTRACT(vk_shader_module)
#define TVKRENDERPASS				_ABSTRACT(vk_render_pass)

#define TVKPHYSICALDEVICEPROPERTIES _OBJ(_I32 _I32 _I32 _I32 _I32 _BYTES _BYTES)
#define TVKDEVICEQUEUEFAMILYPROPERTIES _OBJ(_I32 _I32 _I32 _OBJ(_I32 _I32 _I32))

#ifndef VK_KHR_SURFACE_EXTENSION_NAME
#define VK_KHR_SURFACE_EXTENSION_NAME	"VK_KHR_surface"
#endif

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

	const char **extensionNames = malloc(sizeof(const char *) * (extensionCount + 1));
    extensionNames[0] = VK_KHR_SURFACE_EXTENSION_NAME;
	if(!extensionNames) {
		hl_error("Fatal : Cannot allocate extensionNames\n");
		return instance;
	}

	if(!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, &extensionNames[1])) {
		hl_error("Fatal : SDL_Vulkan_GetInstanceExtensions (2) error\n");
		free(extensionNames);
		return instance;
	}

	// Now we can finish filling VkInstanceCreateInfo structure
	createInfo.enabledExtensionCount = extensionCount + 1;
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
    
    const char* pDevExt[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
 	VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.enabledExtensionCount = 1;//ARRAY_SIZE_IN_ELEMENTS(pDevExt);
    deviceCreateInfo.ppEnabledExtensionNames = pDevExt;
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


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Surface API
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HL_PRIM VkSurfaceKHR *HL_NAME(vk_create_surface)(SDL_Window* window, VkInstance* instance, int* width, int* height)
{
	VkSurfaceKHR* surface = malloc(sizeof(VkSurfaceKHR));
	SDL_bool result = SDL_Vulkan_CreateSurface(window, *instance, surface);
	printf("SDL_Vulkan_CreateSurface result=%d\n", result);
	SDL_GL_GetDrawableSize/*SDL_Vulkan_GetDrawableSize*/(window, width, height);
	//printf("SDL_Vulkan_GetDrawableSize w=%d h=%d\n",*width, *height);
	printf("SDL_GL_GetDrawableSize w=%d h=%d\n",*width, *height);
	return surface;
}

HL_PRIM VkSurfaceCapabilitiesKHR * HL_NAME(vk_get_physical_device_surface_capabilities_KHR)(VkPhysicalDevice* device, VkSurfaceKHR* surface)
{
	VkSurfaceCapabilitiesKHR* caps = (VkSurfaceCapabilitiesKHR*)malloc(sizeof(VkSurfaceCapabilitiesKHR)); 
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device, *surface, caps));
	return caps;
}

static uint32_t vk_get_physical_device_surface_formats_KHR(VkPhysicalDevice* device, VkSurfaceKHR* surface, VkSurfaceFormatKHR** ppSurfaceFormats)
{
	uint32_t nSurfaceFormatCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(*device, *surface, &nSurfaceFormatCount, NULL));
	*ppSurfaceFormats = malloc(sizeof(VkSurfaceFormatKHR*) * nSurfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(*device, *surface, &nSurfaceFormatCount, *ppSurfaceFormats);
	return nSurfaceFormatCount;
}

HL_PRIM VkSurfaceFormatKHR *HL_NAME(vk_get_physical_device_surface_formats_KHR_next)(VkPhysicalDevice* device, VkSurfaceKHR* surface)
{
	static VkSurfaceFormatKHR* pSurfaceFormats = NULL;
	static uint32_t idxSurfaceFormats = 0;
	static uint32_t nSurfaceFormats = 0;
	static VkPhysicalDevice* pPhysicalDevice = NULL;
	static VkSurfaceKHR* pSurface = NULL; 
	if ((pSurfaceFormats == NULL) || (device != pPhysicalDevice) || (surface != pSurface)) {
		pPhysicalDevice = device;
		pSurface = surface;
		nSurfaceFormats = vk_get_physical_device_surface_formats_KHR(device, surface, &pSurfaceFormats);
	}
	if (idxSurfaceFormats < nSurfaceFormats)
		return &pSurfaceFormats[idxSurfaceFormats++];
	else {
		free(pSurfaceFormats);
		pSurfaceFormats = NULL;
		idxSurfaceFormats = nSurfaceFormats = 0;
		return NULL;
	}
}

static uint32_t vk_get_physical_device_surface_present_modes_KHR(VkPhysicalDevice* device, VkSurfaceKHR* surface, VkPresentModeKHR** ppPresentModes)
{
	uint32_t nPresentModeCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(*device, *surface, &nPresentModeCount, NULL));
	*ppPresentModes = malloc(sizeof(VkPresentModeKHR*) * nPresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(*device, *surface, &nPresentModeCount, *ppPresentModes);
	return nPresentModeCount;
}

HL_PRIM VkPresentModeKHR *HL_NAME(vk_get_physical_device_surface_present_modes_KHR_next)(VkPhysicalDevice* device, VkSurfaceKHR* surface)
{
	static VkPresentModeKHR* pPresentModes = NULL;
	static uint32_t idxPresentMode = 0;
	static uint32_t nPresentModeCount = 0;
	static VkPhysicalDevice* pPhysicalDevice = NULL;
	static VkSurfaceKHR* pSurface = NULL; 
	if ((pPresentModes == NULL) || (device != pPhysicalDevice) || (surface != pSurface)) {
		pPhysicalDevice = device;
		pSurface = surface;
		nPresentModeCount = vk_get_physical_device_surface_present_modes_KHR(device, surface, &pPresentModes);
	}
	if (idxPresentMode < nPresentModeCount)
		return &pPresentModes[idxPresentMode++];
	else {
		free(pPresentModes);
		pPresentModes = NULL;
		idxPresentMode = nPresentModeCount = 0;
		return NULL;
	}
}

static uint32_t vk_get_swapchain_images_KHR(VkDevice* device, VkSwapchainKHR* swapchain, VkImage** ppImages)
{
	uint32_t nImageCount;
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(*device, *swapchain, &nImageCount, NULL));
	*ppImages = malloc(sizeof(VkImage*) * nImageCount);
	vkGetSwapchainImagesKHR(*device, *swapchain, &nImageCount, *ppImages);
	return nImageCount;
}

HL_PRIM VkImage *HL_NAME(vk_get_swapchain_images_KHR_next)(VkDevice* device, VkSwapchainKHR* swapchain)
{
	static VkImage* pImages = NULL;
	static uint32_t idxImage = 0;
	static uint32_t nImageCount = 0;
	static VkDevice* pDevice = NULL;
	static VkSwapchainKHR* pSwapchain = NULL; 
	if ((pImages == NULL) || (device != pDevice) || (swapchain != pSwapchain)) {
		pDevice = device;
		pSwapchain = swapchain;
		nImageCount = vk_get_swapchain_images_KHR(device, swapchain, &pImages);
	}
	if (idxImage < nImageCount)
		return &pImages[idxImage++];
	else {
		free(pImages);
		pImages = NULL;
		idxImage = nImageCount = 0;
		return NULL;
	}
}

HL_PRIM VkSwapchainKHR *HL_NAME(vk_create_swapchain_KHR)(VkDevice* device, VkSurfaceKHR* surface)
{
	VkSwapchainKHR* swapChain = (VkSwapchainKHR*)malloc(sizeof(VkSwapchainKHR));
	VkSwapchainCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *surface;
	createInfo.minImageCount = 2;
	createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	createInfo.imageArrayLayers = 1;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = true;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	VK_CHECK_RESULT(vkCreateSwapchainKHR(*device, &createInfo, NULL, swapChain));
	return swapChain;
}

HL_PRIM void HL_NAME(vk_destroy_swapchain_KHR)(VkDevice* device, VkSwapchainKHR* swapchain)
{
	vkDestroySwapchainKHR(*device, *swapchain, NULL);
}

HL_PRIM VkImageView *HL_NAME(vk_create_image_view)(VkDevice* device, VkImage* image)
{
	VkImageView* vkImageView = malloc(sizeof(VkImageView));
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = *image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = VK_FORMAT_B8G8R8A8_UNORM; //swapChainImageFormat;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	VK_CHECK_RESULT(vkCreateImageView(*device, &createInfo, NULL, vkImageView));
	return vkImageView;
}

HL_PRIM void HL_NAME(vk_destroy_image_view)(VkDevice* device, VkImageView* imageView)
{
	vkDestroyImageView(*device, *imageView, NULL);
}

HL_PRIM VkShaderModule *HL_NAME(vk_create_shader_module)(VkDevice* device, const void* data, int size)
{
	VkShaderModule* shaderModule = malloc(sizeof(VkShaderModule));
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	createInfo.pCode = (const uint32_t*)data;
	VK_CHECK_RESULT(vkCreateShaderModule(*device, &createInfo, NULL, shaderModule));
	return shaderModule;
}

HL_PRIM void HL_NAME(vk_destroy_shader_module)(VkDevice* device, VkShaderModule* module)
{
	vkDestroyShaderModule(*device, *module, NULL);
}

HL_PRIM VkRenderPass *HL_NAME(vk_create_render_pass)(VkDevice* device)
{
	// TODO
	return NULL;
}

HL_PRIM void HL_NAME(vk_destroy_render_pass)(VkDevice* device, VkRenderPass* renderPass)
{
	// TODO
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

DEFINE_PRIM(TVKSURFACEKHR, vk_create_surface, TSDLWINDOW TVKINSTANCE _REF(_I32) _REF(_I32))

DEFINE_PRIM(TVKSURFACECAPABILITIESKHR, vk_get_physical_device_surface_capabilities_KHR, TVKPHYSICALDEVICE TVKSURFACEKHR)
DEFINE_PRIM(TVKSURFACEFORMATKHR, vk_get_physical_device_surface_formats_KHR_next, TVKPHYSICALDEVICE TVKSURFACEKHR)
DEFINE_PRIM(TVKPRESENTMODEKHR, vk_get_physical_device_surface_present_modes_KHR_next, TVKPHYSICALDEVICE TVKSURFACEKHR)

DEFINE_PRIM(TVKSWAPCHAINKHR, vk_create_swapchain_KHR, TVKDEVICE TVKSURFACEKHR)
DEFINE_PRIM(_VOID, vk_destroy_swapchain_KHR, TVKDEVICE TVKSWAPCHAINKHR)
DEFINE_PRIM(TVKIMAGE, vk_get_swapchain_images_KHR_next, TVKDEVICE TVKSWAPCHAINKHR)

DEFINE_PRIM(TVKIMAGEVIEW, vk_create_image_view, TVKDEVICE TVKIMAGE)
DEFINE_PRIM(_VOID, vk_destroy_image_view, TVKDEVICE TVKIMAGEVIEW)

DEFINE_PRIM(TVKSHADERMODULE, vk_create_shader_module, TVKDEVICE _BYTES _I32)
DEFINE_PRIM(_VOID, vk_destroy_shader_module, TVKDEVICE TVKSHADERMODULE)

DEFINE_PRIM(TVKRENDERPASS, vk_create_render_pass, TVKDEVICE)
DEFINE_PRIM(_VOID, vk_destroy_render_pass, TVKDEVICE TVKRENDERPASS)
#endif
