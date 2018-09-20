package vulkan;

import vulkan.VkInstance;
import hl.NativeArray;

private typedef VkInstancePtr = hl.Abstract<"vk_instance">;
private typedef VkPhysicalDevicePtr = hl.Abstract<"vk_physical_device">;
private typedef VkDevicePtr = hl.Abstract<"vk_device">;

@:hlNative("vulkan")
class VkDevice {

	var ptr : VkDevicePtr;
	var physicalDevice : VkPhysicalDevice;

	public function new( physDevice : VkPhysicalDevice ) {
		physicalDevice = physDevice;
		ptr = vkCreateDevice(physicalDevice.getPtr(), 0, 1); // TODO handle queueIndex / queueCount ??
	}

	public function destroy():Void {
		vkDestroyDevice( ptr );
	}

	@:hlNative("vulkan","vk_destroy_device")
	private static function vkDestroyDevice( devicePtr : VkDevicePtr) : Void { }

	@:hlNative("vulkan","vk_create_device")
	private static function vkCreateDevice( physicalDevicePtr : VkPhysicalDevicePtr, iQueueFamilyIndex : Int, nQueueCount : Int ) : VkDevicePtr {
		return null;
	}
}
