package vulkan;

import vulkan.VkInstance;

private typedef VkInstancePtr = hl.Abstract<"vk_instance">;
private typedef VkPhysicalDevicePtr = hl.Abstract<"vk_physical_device">;

@:hlNative("vulkan")
class VkPhysicalDevice {

	var ptr : VkPhysicalDevicePtr;

	public function new( p : VkPhysicalDevicePtr ) {
		ptr = p;
	}

	public static function enumerate( instance : VkInstance) : Array<VkPhysicalDevice> {
		var devices : Array<VkPhysicalDevice> = new Array<VkPhysicalDevice>();
		var d : VkPhysicalDevice;
		do {
			d = enumerateNext(instance);
			if (d != null)
				devices.push(d);
		} while (d != null);
		return devices;
	}
	
	private static function enumerateNext( instance : VkInstance ) : VkPhysicalDevice {
		var instancePtr : VkInstancePtr = instance.getPtr();
		var p : VkPhysicalDevicePtr = vkEnumeratePhysicalDeviceNext( instancePtr );
		if (p == null)
			return null;
		var d : VkPhysicalDevice = new VkPhysicalDevice(p);
		return d;
	}

	@:hlNative("vulkan","vk_enumerate_physical_device_next")
	private static function vkEnumeratePhysicalDeviceNext( instancePtr : VkInstancePtr ) : VkPhysicalDevicePtr {
		return null;
	}
}
