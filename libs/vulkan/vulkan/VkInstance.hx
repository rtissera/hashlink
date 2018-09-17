package vulkan;

import sdl.Window;

private typedef VkInstancePtr = hl.Abstract<"vk_instance">;
private typedef WinPtr = hl.Abstract<"sdl_window">;

@:hlNative("vulkan")
class VkInstance {

	var ptr : VkInstancePtr;

	public function getPtr() : VkInstancePtr {
		return ptr;
	}

	public function new(w : sdl.Window) {
		if (vkInit() == false) {
			throw "No Vulkan support";
		}
		ptr = vkCreateInstance(w.getWin());
	}

	public function destroy() {
		vkDestroyInstance( ptr );
	}

	@:hlNative("vulkan", "vk_init")
	private static function vkInit() : Bool { return false; }

	@:hlNative("vulkan", "vk_create_instance")
	private static function vkCreateInstance(w : WinPtr) : VkInstancePtr {
		return null;
	}

	@:hlNative("vulkan", "vk_destroy_instance")
	private static function vkDestroyInstance( instance : VkInstancePtr ) : Void { }

}
