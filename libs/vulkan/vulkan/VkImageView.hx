package vulkan;

import sdl.Window;
import vulkan.VkSwapChain;
import vulkan.VkDevice;

typedef VkImageViewPtr = hl.Abstract<"vk_image_view">;

@:hlNative("vulkan")
class VkImageView {

	var ptr : VkImageViewPtr;
	var device : VkDevice;

	public function getPtr() : VkImageViewPtr {
		return ptr;
	}

	public function new(d : VkDevice, image : VkImagePtr) {
		device = d;
		ptr = vkCreateImageView(d.getPtr(), image);
	}

	public function destroy() {
		vkDestroyImageView(device.getPtr(), ptr);
	}

	@:hlNative("vulkan", "vk_create_image_view")
	private static function vkCreateImageView(devicePtr : VkDevicePtr, imagePtr : VkImagePtr) : VkImageViewPtr {
		return null;
	}

	@:hlNative("vulkan", "vk_destroy_image_view")
	private static function vkDestroyImageView(devicePtr : VkDevicePtr, imageViewPtr : VkImageViewPtr) : Void {
	}

}
