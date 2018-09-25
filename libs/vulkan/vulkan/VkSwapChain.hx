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
@:hlNative("vulkan")
class VkSwapChain {


	var device : VkDevice;
	var surface : VkSurface;

	var capabilities : VkSurfaceCapabilitiesPtr;
	var formats : Array<VkSurfaceFormatPtr>;
	var presentModes : Array<VkPresentModePtr>;
	var ptr : VkSwapChainPtr;

	public function getPtr() : VkSwapChainPtr {
		return ptr;
	}

	public function getDevice() : VkDevice {
		return device;
	}

	public function getSurface() : VkSurface {
		return surface;
	}

	public function new(s : VkSurface, d : VkDevice) {
		surface = s;
		device = d;
		var physDevice : VkPhysicalDevice = device.getPhysicalDevice();
		retrieveCapabilities(physDevice, surface);
		enumerateFormats(physDevice.getPtr(), surface.getPtr());
		enumerateModes(physDevice.getPtr(), surface.getPtr());
		createSwapChain();
	}

	private function retrieveCapabilities(physDevice : VkPhysicalDevice, surface : VkSurface) {
		capabilities = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice.getPtr(), surface.getPtr());
	}

	private function enumerateFormats(physDevicePtr : VkPhysicalDevicePtr, surfacePtr : VkSurfacePtr) {
		formats = new Array<VkSurfaceFormatPtr>();
		var ptr : VkSurfaceFormatPtr = null;
		do {
			ptr = vkGetPhysicalDeviceSurfaceFormatsKHR(physDevicePtr, surfacePtr);
			if (ptr != null)
				formats.push(ptr);
		} while (ptr != null);
	}

	private function enumerateModes(physDevicePtr : VkPhysicalDevicePtr, surfacePtr : VkSurfacePtr) {
		presentModes = new Array<VkPresentModePtr>();
		var ptr : VkPresentModePtr = null;
		do {
			ptr = vkGetPhysicalDeviceSurfacePresentModesKHR(physDevicePtr, surfacePtr);
			if (ptr != null)
				presentModes.push(ptr);
		} while (ptr != null);
	}

	private function createSwapChain() {
		// TODO reuse queried data earlier
		ptr = vkCreateSwapChainKHR(device.getPtr(), surface.getPtr());
	}

	public function destroy() {
		// TODO 
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
