import vulkan.VkInstance;
import vulkan.VkPhysicalDevice;
import vulkan.VkDevice;

import sdl.Sdl;
import sdl.Window;

class Main {
	
	static public function main():Void {
		trace("Hello World");
		Sdl.init();
		trace("SDL OK");
		var w : Window = new Window("VULKAN TEST", 640, 480);
		w.maximize();
		trace("WINDOW OK");
		var instance : VkInstance = new VkInstance(w);
		trace("VkInstance created");
		var physicalDevices : Array<VkPhysicalDevice> = VkPhysicalDevice.enumerate(instance);
		trace("Found " + physicalDevices.length + " physical devices !");
		var devices:Array<VkDevice> = new Array<VkDevice>();
		for (physicalDevice in physicalDevices)
		{
			devices.push(new VkDevice(physicalDevice));
		}
		var t : Int = 0;
		while (t < 10) {
			Sys.sleep(1);
			w.present();
			t++;
		}
		for (d in devices)
			d.destroy();
		instance.destroy();
		trace("VkInstance destroyed");
	}
}

