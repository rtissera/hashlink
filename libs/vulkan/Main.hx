import vulkan.VkInstance;
import vulkan.VkPhysicalDevice;
import vulkan.VkDevice;
import vulkan.VkQueue;
import vulkan.VkSurface;
import vulkan.VkSwapChain;
import vulkan.VkImageView;
import vulkan.VkShaderModule;

import haxe.io.Bytes;

import sdl.Sdl;
import sdl.Window;

class Main {
	
	static public function main():Void {

		// Init SDL window
		trace("Hello World");
		Sdl.init();
		trace("SDL OK");
		var w : Window = new Window("VULKAN TEST", 640, 480);
		w.maximize();
		trace("WINDOW OK");

		// Build Vulkan objects
		var instance : VkInstance = new VkInstance(w);
		trace("VkInstance created");
		var physicalDevices : Array<VkPhysicalDevice> = VkPhysicalDevice.enumerate(instance);
		trace("Found " + physicalDevices.length + " physical devices !");
		var devices:Array<VkDevice> = new Array<VkDevice>();
		for (physicalDevice in physicalDevices)
		{
			devices.push(new VkDevice(physicalDevice));
		}
		var queues:Array<VkQueue> = new Array<VkQueue>();
		for (d in devices)
		{
			var q : VkQueue = new VkQueue(d, 0, 0); // 1 queue, first index
			queues.push(q);
		}
		var t : Int = 0;
		var surface : VkSurface = new VkSurface(w, instance);
		var swapchain : VkSwapChain = new VkSwapChain(surface, devices[0]);
		var images : Array<VkImagePtr> = swapchain.getImages();
		var imageViews : Array<VkImageView> = new Array<VkImageView>();
		for (i in images)
			imageViews.push(new VkImageView(devices[0], i));

		var vertexShader : VkShaderModule = new VkShaderModule(devices[0], sys.io.File.getBytes("shaders/vertexShader"));
		var fragmentShader : VkShaderModule = new VkShaderModule(devices[0], sys.io.File.getBytes("shaders/fragmentShader"));

		// Pseudo rendering...
		while (t < 10) {
			Sys.sleep(0.1);
			w.present();
			for (d in devices)
				trace("Device WaitIdle result="+d.waitIdle());
			for (q in queues)
				trace("Queue WaitIdle result="+q.waitIdle());
			t++;
		}

		// Destroy 
		vertexShader.destroy();
		fragmentShader.destroy();
		for (i in imageViews)
			i.destroy();
		swapchain.destroy();
		for (d in devices)
			d.destroy();
		instance.destroy();
		trace("VkInstance destroyed");
	}
}

