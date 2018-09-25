package vulkan;

import sdl.Window;
import vulkan.VkInstance;

typedef VkSurfacePtr = hl.Abstract<"vk_surface_khr">;
private typedef VkInstancePtr = hl.Abstract<"vk_instance">;
private typedef WinPtr = hl.Abstract<"sdl_window">;

@:hlNative("vulkan")
class VkSurface {

	var ptr : VkSurfacePtr;
	var width : Int;
	var height : Int;

	public function getPtr() : VkSurfacePtr {
		return ptr;
	}

	public function new(w : sdl.Window, instance : VkInstance) {
		var _w = new hl.Ref<Int>(width);
		var _h = new hl.Ref<Int>(height);
		ptr = vkCreateSurface(w.getWin(), instance.getPtr(), _w, _h);
		width = _w.get();
		height = _h.get();
		trace("VKSurface created width=" + width + ", height=" + height);
		trace("SDLWindow width="+w.width+", height="+w.height);
	}

	public function destroy() {
		// TODO 
	}

	@:hlNative("vulkan", "vk_create_surface")
	private static function vkCreateSurface(w : WinPtr, instancePtr : VkInstancePtr, width : hl.Ref<Int>, height : hl.Ref<Int>) : VkSurfacePtr {
		return null;
	}

}
