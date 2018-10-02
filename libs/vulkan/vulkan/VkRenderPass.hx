package vulkan;

import vulkan.VkDevice;

private typedef VkRenderPassPtr = hl.Abstract<"vk_render_pass">;

@:hlNative("vulkan")
class VkRenderPass {

	var device : VkDevice;
	var ptr : VkRenderPassPtr;

	public function new(d : VkDevice) {
		device = d;
		ptr = vkCreateRenderPass(device);
	}

	@:hlNative("vulkan", "vk_create_render_pass")
	private static function vkCreateRenderPass(devicePtr : VkDevicePtr) : VkRenderPassPtr {
		return null;
	}

	@:hlNative("vulkan", "vk_destroy_render_pass")
	private static function vkDestroyImageView(devicePtr : VkDevicePtr, renderPassPtr : VkRenderPassPtr) : Void {
	}

}