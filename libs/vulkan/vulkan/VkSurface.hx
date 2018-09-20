package vulkan;

import sdl.Window;
import vulkan.VkInstance;

private typedef VkSurfacePtr = hl.Abstract<"vk_surface_khr">;
private typedef VkInstancePtr = hl.Abstract<"vk_instance">;
private typedef WinPtr = hl.Abstract<"sdl_window">;

@:hlNative("vulkan")
class VkSurface {

	var ptr : VkSurfacePtr;
	var width : haxe.Int32;
	var height : haxe.Int32;

	public function getPtr() : VkSurfacePtr {
		return ptr;
	}

	public function new(w : sdl.Window, instance : VkInstance) {
		ptr = vkCreateSurface(w.getWin(), instance.getPtr(), width, height);
		trace("VKSurface created width=" + width + ", height=" + height);
		trace("SDLWindow width="+w.width+", height="+w.height);
	}

	public function destroy() {
		// TODO 
	}

	@:hlNative("vulkan", "vk_create_surface")
	private static function vkCreateSurface(w : WinPtr, instancePtr : VkInstancePtr, width : hl.Ref<haxe.Int32>, height : hl.Ref<haxe.Int32>) : VkSurfacePtr {
		return null;
	}

}
