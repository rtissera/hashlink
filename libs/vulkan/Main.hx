import vulkan.VkInstance;
import vulkan.VkPhysicalDevice;

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
		var physicalDevices : Array<VkPhysicalDevice> = VkPhysicalDevice.enumerate(instance);
		trace("Found " + physicalDevices.length + " physical devices !");
		instance.destroy();
		trace("VkInstance destroyed");
	}
}

