package vulkan;

import vulkan.VkDevice;
import haxe.io.Bytes;

typedef VkShaderModulePtr = hl.Abstract<"vk_shader_module">;

@:hlNative("vulkan")
class VkShaderModule {

	var ptr : VkShaderModulePtr;

	var device : VkDevice;
	var code : Bytes;

	public function getPtr() : VkShaderModulePtr {
		return ptr;
	}

	public function new(d : VkDevice, c : Bytes) {
		device = d;
		code = c;
		ptr = vkCreateShaderModule(device.getPtr(), hl.Bytes.fromBytes(code));
	}

	public function destroy() {
		vkDestroyShaderModule(device.getPtr(), ptr);
	}

	@:hlNative("vulkan", "vk_create_shader_module")
	private static function vkCreateShaderModule(devicePtr : VkDevicePtr, code : hl.Bytes) : VkShaderModulePtr {
		return null;
	}

	@:hlNative("vulkan", "vk_destroy_shader_module")
	private static function vkDestroyShaderModule(devicePtr : VkDevicePtr, shaderModulePtr : VkShaderModulePtr) : Void {
	}

}
