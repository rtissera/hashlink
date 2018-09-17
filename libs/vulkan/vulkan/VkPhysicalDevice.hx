package vulkan;

import vulkan.VkInstance;

private typedef VkInstancePtr = hl.Abstract<"vk_instance">;
private typedef VkPhysicalDevicePtr = hl.Abstract<"vk_physical_device">;

private class VkPhysicalDeviceProperties {
	public var apiVersion : Int;
	public var driverVersion : Int;
	public var vendorID : Int;
	public var deviceID : Int;
	public var deviceType : VkPhysicalDeviceType;
	public var deviceName : hl.Bytes;
	public var pipelineCacheUUID : hl.Bytes; 

	public function new() {
		pipelineCacheUUID = new hl.Bytes(VkPhysicalDevice.VK_UUID_SIZE);
	}
}

@:enum abstract VkPhysicalDeviceType(Int) {
	var VK_PHYSICAL_DEVICE_TYPE_OTHER = 0;
	var VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1;
	var VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2;
	var VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3;
	var VK_PHYSICAL_DEVICE_TYPE_CPU = 4;
}

@:enum abstract VkBool(Int) {
	var VK_FALSE = 0;
	var VK_TRUE = 1;
}


@:hlNative("vulkan")
class VkPhysicalDevice {
	public static var VK_UUID_SIZE : Int = 16;
	public static var VK_MAX_PHYSICAL_DEVICE_NAME_SIZE : Int = 256;

	var ptr : VkPhysicalDevicePtr;
	var properties : VkPhysicalDeviceProperties;
	
	var apiVersion : Int;
	var driverVersion : Int;
	var vendorID : Int;
	var deviceID : Int;
	var deviceType : VkPhysicalDeviceType;
	var deviceName : String;
	var pipelineCacheUUID : String;

	public function new( p : VkPhysicalDevicePtr ) {
		ptr = p;
		properties = new VkPhysicalDeviceProperties();
		vkGetPhysicalDeviceProperties(ptr, properties);
		retrieveProperties();
		printProperties();
	}

	private function retrieveProperties() {
		apiVersion = properties.apiVersion;
		driverVersion = properties.driverVersion;
		vendorID = properties.vendorID;
		deviceID = properties.deviceID;
		deviceType = properties.deviceType;
		deviceName = @:privateAccess String.fromUTF8(properties.deviceName);
		var bytes : haxe.io.Bytes = @:privateAccess new haxe.io.Bytes(properties.pipelineCacheUUID, VK_UUID_SIZE);
		pipelineCacheUUID = bytes.toHex();
	}
	
	public function printProperties() : Void {
		trace("apiVersion " + apiVersion);
		trace("driverVersion " + driverVersion);
		trace("vendorID " + vendorID);
		trace("deviceID " + deviceID);
		trace("deviceType " + deviceType);
		trace("deviceName " + deviceName);
		trace("pipelineCacheUUID " + pipelineCacheUUID);
	}

	public static function enumerate( instance : VkInstance) : Array<VkPhysicalDevice> {
		var devices : Array<VkPhysicalDevice> = new Array<VkPhysicalDevice>();
		var d : VkPhysicalDevice;
		do {
			d = enumerateNext(instance);
			if (d != null)
				devices.push(d);
		} while (d != null);
		return devices;
	}
	
	private static function enumerateNext( instance : VkInstance ) : VkPhysicalDevice {
		var instancePtr : VkInstancePtr = instance.getPtr();
		var p : VkPhysicalDevicePtr = vkEnumeratePhysicalDeviceNext( instancePtr );
		if (p == null)
			return null;
		var d : VkPhysicalDevice = new VkPhysicalDevice(p);
		return d;
	}

	@:hlNative("vulkan","vk_enumerate_physical_device_next")
	private static function vkEnumeratePhysicalDeviceNext( instancePtr : VkInstancePtr ) : VkPhysicalDevicePtr {
		return null;
	}

	@:hlNative("vulkan","vk_get_physical_device_properties")
	private static function vkGetPhysicalDeviceProperties( devicePtr : VkPhysicalDevicePtr, properties : VkPhysicalDeviceProperties) : Void {
	}
}
