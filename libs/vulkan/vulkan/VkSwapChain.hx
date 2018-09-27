package vulkan;

import sdl.Window;
import vulkan.VkInstance;
import vulkan.VkSurface;
import vulkan.VkDevice;
import vulkan.VkPhysicalDevice;

private typedef VkSurfaceCapabilitiesPtr = hl.Abstract<"vk_surface_capabilities_khr">; //VkSurfaceCapabilitiesKHR
private typedef VkSurfaceFormatPtr = hl.Abstract<"vk_surface_format_khr">; //VkSurfaceFormatKHR
private typedef VkPresentModePtr = hl.Abstract<"vk_present_mode_khr">; //VkPresentModeKHR
private typedef VkSwapChainPtr = hl.Abstract<"vk_swapchain_khr">; //VkSwapChainKHR
typedef VkImagePtr = hl.Abstract<"vk_image">; // VkImage

@:hlNative("vulkan")
class VkSwapChain {

	var ptr : VkSwapChainPtr;

	var device : VkDevice;
	var surface : VkSurface;

	var capabilities : VkSurfaceCapabilitiesPtr;
	var formats : Array<VkSurfaceFormatPtr>;
	var presentModes : Array<VkPresentModePtr>;

	var images : Array<VkImagePtr>;

	public function getPtr() : VkSwapChainPtr {
		return ptr;
	}

	public function getDevice() : VkDevice {
		return device;
	}

	public function getSurface() : VkSurface {
		return surface;
	}

	public function getImages() : Array<VkImagePtr> {
		return images;
	}

	public function new(s : VkSurface, d : VkDevice) {
		surface = s;
		device = d;
		var physDevice : VkPhysicalDevice = device.getPhysicalDevice();
		retrieveCapabilities(physDevice, surface);
		enumerateFormats(physDevice.getPtr(), surface.getPtr());
		enumerateModes(physDevice.getPtr(), surface.getPtr());
		createSwapChain();
		retrieveImages();
	}

	private function retrieveCapabilities(physDevice : VkPhysicalDevice, surface : VkSurface) {
		capabilities = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice.getPtr(), surface.getPtr());
	}

	private function enumerateFormats(physDevicePtr : VkPhysicalDevicePtr, surfacePtr : VkSurfacePtr) {
		formats = new Array<VkSurfaceFormatPtr>();
		var surfaceFormatPtr : VkSurfaceFormatPtr = null;
		do {
			surfaceFormatPtr = vkGetPhysicalDeviceSurfaceFormatsKHR(physDevicePtr, surfacePtr);
			if (surfaceFormatPtr != null)
				formats.push(surfaceFormatPtr);
		} while (surfaceFormatPtr != null);
	}

	private function enumerateModes(physDevicePtr : VkPhysicalDevicePtr, surfacePtr : VkSurfacePtr) {
		presentModes = new Array<VkPresentModePtr>();
		var presentModePtr : VkPresentModePtr = null;
		do {
			presentModePtr = vkGetPhysicalDeviceSurfacePresentModesKHR(physDevicePtr, surfacePtr);
			if (presentModePtr != null)
				presentModes.push(presentModePtr);
		} while (presentModePtr != null);
	}

	private function createSwapChain() {
		// TODO reuse queried data earlier
		ptr = vkCreateSwapChainKHR(device.getPtr(), surface.getPtr());
	}

	private function retrieveImages() {
		images = new Array<VkImagePtr>();
		var imagePtr : VkImagePtr = null;
		do {
			imagePtr = vkGetSwapchainImagesKHR(device.getPtr(), ptr);
			if (imagePtr != null)
				images.push(imagePtr);
		} while (imagePtr != null);
	}

	public function destroy() {
		vkDestroySwapchainKHR(device.getPtr(), ptr);
	}

	@:hlNative("vulkan", "vk_get_swapchain_images_KHR_next")
	private static function vkGetSwapchainImagesKHR(devicePtr : VkDevicePtr, swapchainPtr : VkSwapChainPtr) : VkImagePtr {
		return null;
	}

	@:hlNative("vulkan", "vk_destroy_swapchain_KHR")
	private static function vkDestroySwapchainKHR(devicePtr : VkDevicePtr, swapchainPtr : VkSwapChainPtr) : Void {
	}

	@:hlNative("vulkan", "vk_get_physical_device_surface_capabilities_KHR")
	private static function vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevicePtr : VkPhysicalDevicePtr, surfacePtr : VkSurfacePtr) : VkSurfaceCapabilitiesPtr {
		return null;
	}

	@:hlNative("vulkan", "vk_get_physical_device_surface_formats_KHR_next")
	private static function vkGetPhysicalDeviceSurfaceFormatsKHR(physDevicePtr : VkPhysicalDevicePtr, surfacePtr : VkSurfacePtr) : VkSurfaceFormatPtr {
		return null;
	}

	@:hlNative("vulkan", "vk_get_physical_device_surface_present_modes_KHR_next")
	private static function vkGetPhysicalDeviceSurfacePresentModesKHR(physDevicePtr : VkPhysicalDevicePtr, surfacePtr : VkSurfacePtr) : VkPresentModePtr {
		return null;
	}

	@:hlNative("vulkan", "vk_create_swapchain_KHR")
	private static function vkCreateSwapChainKHR(devicePtr : VkDevicePtr, surfacePtr : VkSurfacePtr) : VkSwapChainPtr {
		return null;
	}

}
