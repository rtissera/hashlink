package vulkan;

private typedef VkDevicePtr = hl.Abstract<"vk_device">;
private typedef VkQueuePtr = hl.Abstract<"vk_queue">;

class VkQueue {
	
	var device : VkDevice;
	var ptr : VkQueuePtr;

	public function new(device : VkDevice, queueFamilyIndex : Int, queueIndex : Int)
	{
		ptr = vkGetDeviceQueue(device.getPtr(), queueFamilyIndex, queueIndex);
	}

	public function waitIdle() : haxe.Int32 {
		return vkQueueWaitIdle(ptr);
	}

	@:hlNative("vulkan","vk_get_device_queue")
	private static function vkGetDeviceQueue(devicePtr : VkDevicePtr, queueFamilyIndex : Int, queueIndex : Int) : VkQueuePtr {
		return null;
	}

	@:hlNative("vulkan","vk_queue_wait_idle")
	private static function vkQueueWaitIdle( queuePtr : VkQueuePtr) : haxe.Int32 {
		return 0;
	}
}