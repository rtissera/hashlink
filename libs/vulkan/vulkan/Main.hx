import vulkan.VkInstance;

class Main {
	
	static public function main():Void {
		trace("Hello World");
		var instance : VkInstance = new VkInstance();
		trace("VkInstance created");
		instance.destroy();
		trace("VkInstance destroyed");
	}
}

