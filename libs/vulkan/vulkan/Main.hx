import vulkan.VkInstance;
import sdl.Sdl;
import sdl.Window;

class Main {
	
	static public function main():Void {
		trace("Hello World");
		Sdl.init();
		trace("SDL OK");
		var w : Window = new Window("VULKAN TEST", 640, 480);
		trace("WINDOW OK");
		var instance : VkInstance = new VkInstance(w);
		trace("VkInstance created");
		instance.destroy();
		trace("VkInstance destroyed");
	}
}

