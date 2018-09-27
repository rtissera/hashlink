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

	public function new(d : VkDevice, c : haxe.io.Bytes) {
		device = d;
		code = c;
		var data : hl.Bytes = hl.Bytes.fromBytes(code);
		ptr = vkCreateShaderModule(device.getPtr(), data, c.length);
	}

	public function destroy() {
		vkDestroyShaderModule(device.getPtr(), ptr);
	}

	@:hlNative("vulkan", "vk_create_shader_module")
	private static function vkCreateShaderModule(devicePtr : VkDevicePtr, code : hl.Bytes, len : Int) : VkShaderModulePtr {
		return null;
	}

	@:hlNative("vulkan", "vk_destroy_shader_module")
	private static function vkDestroyShaderModule(devicePtr : VkDevicePtr, shaderModulePtr : VkShaderModulePtr) : Void {
	}

}
