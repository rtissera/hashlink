package vulkan;

private typedef VkInstancePtr = hl.Abstract<"vk_instance">;
private typedef VkPhysicalDevicePtr = hl.Abstract<"vk_physical_device">;

@:hlNative("vulkan")
class VkPhysicalDevice {

	var ptr : VkPhysicalDevicePtr;

	public function new( p : VkPhysicalDevicePtr ) {
		ptr = p;
	}

	public static function enumerate( instancePtr : VkInstancePtr ) : Array<VkPhysicalDevice> {
		var devices : Array<VkPhysicalDevice> = new Array<VkPhysicalDevice>();
		Array<VkPhysicalDevicePtr> array = vkEnumeratePhysicalDevices( instancePtr );
		for (ptr in devices) {
			VkPhysicalDevice d = new VkPhysicalDevice(d);
			devices.push(d);
		}
		return devices;
	}

	private static function vkEnumeratePhysicalDevices() : Array<VkPhysicalDevicePtr> {
		return null;
	}
}
