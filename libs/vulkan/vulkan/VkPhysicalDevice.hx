package vulkan;

import vulkan.VkInstance;
import hl.NativeArray;

private typedef VkInstancePtr = hl.Abstract<"vk_instance">;
private typedef VkPhysicalDevicePtr = hl.Abstract<"vk_physical_device">;

private class VkPhysicalDeviceProperties {
	public var apiVersion : haxe.Int32;
	public var driverVersion : haxe.Int32;
	public var vendorID : haxe.Int32;
	public var deviceID : haxe.Int32;
	public var deviceType : VkPhysicalDeviceType;
	public var deviceName : hl.Bytes;
	public var pipelineCacheUUID : hl.Bytes; 
	//public var limits : VkPhysicalDeviceLimits;
	//public var sparseProperties : VkPhysicalDeviceSparseProperties;

	public function new() {
	}
}

typedef Size_t = haxe.Int64; // ???
typedef VkDeviceSize = haxe.Int64;
typedef VkBool32 = haxe.Int32;

typedef VkFlags = haxe.Int32;
typedef VkSampleCountFlags = VkFlags;
typedef VkQueueFlags = VkFlags;

private class VkPhysicalDeviceLimits {
	public var maxImageDimension1D : haxe.Int32;
	public var maxImageDimension2D : haxe.Int32;
	public var maxImageDimension3D : haxe.Int32;
	public var maxImageDimensionCube : haxe.Int32;
	public var maxImageArrayLayers : haxe.Int32;
	public var maxTexelBufferElements : haxe.Int32;
	public var maxUniformBufferRange : haxe.Int32;
	public var maxStorageBufferRange : haxe.Int32;
	public var maxPushConstantsSize : haxe.Int32;
	public var maxMemoryAllocationCount : haxe.Int32;
	public var maxSamplerAllocationCount : haxe.Int32;
	public var bufferImageGranularity : VkDeviceSize;
	public var sparseAddressSpaceSize : VkDeviceSize;
	public var maxBoundDescriptorSets : haxe.Int32;
	public var maxPerStageDescriptorSamplers : haxe.Int32;
	public var maxPerStageDescriptorUniformBuffers : haxe.Int32;
	public var maxPerStageDescriptorStorageBuffers : haxe.Int32;
	public var maxPerStageDescriptorSampledImages : haxe.Int32;
	public var maxPerStageDescriptorStorageImages : haxe.Int32;
	public var maxPerStageDescriptorInputAttachments : haxe.Int32;
	public var maxPerStageResources : haxe.Int32;
	public var maxDescriptorSetSamplers : haxe.Int32;
	public var maxDescriptorSetUniformBuffers : haxe.Int32;
	public var maxDescriptorSetUniformBuffersDynamic : haxe.Int32;
	public var maxDescriptorSetStorageBuffers : haxe.Int32;
	public var maxDescriptorSetStorageBuffersDynamic : haxe.Int32;
	public var maxDescriptorSetSampledImages : haxe.Int32;
	public var maxDescriptorSetStorageImages : haxe.Int32;
	public var maxDescriptorSetInputAttachments : haxe.Int32;
	public var maxVertexInputAttributes : haxe.Int32;
    public var maxVertexInputBindings : haxe.Int32;
    public var maxVertexInputAttributeOffset : haxe.Int32;
/* TODO
    uint32_t              maxVertexInputBindingStride;
    uint32_t              maxVertexOutputComponents;
    uint32_t              maxTessellationGenerationLevel;
    uint32_t              maxTessellationPatchSize;
    uint32_t              maxTessellationControlPerVertexInputComponents;
    uint32_t              maxTessellationControlPerVertexOutputComponents;
    uint32_t              maxTessellationControlPerPatchOutputComponents;
    uint32_t              maxTessellationControlTotalOutputComponents;
    uint32_t              maxTessellationEvaluationInputComponents;
    uint32_t              maxTessellationEvaluationOutputComponents;
    uint32_t              maxGeometryShaderInvocations;
    uint32_t              maxGeometryInputComponents;
    uint32_t              maxGeometryOutputComponents;
    uint32_t              maxGeometryOutputVertices;
    uint32_t              maxGeometryTotalOutputComponents;
    uint32_t              maxFragmentInputComponents;
    uint32_t              maxFragmentOutputAttachments;
    uint32_t              maxFragmentDualSrcAttachments;
    uint32_t              maxFragmentCombinedOutputResources;
    uint32_t              maxComputeSharedMemorySize;
    uint32_t              maxComputeWorkGroupCount[3];
    uint32_t              maxComputeWorkGroupInvocations;
    uint32_t              maxComputeWorkGroupSize[3];
    uint32_t              subPixelPrecisionBits;
    uint32_t              subTexelPrecisionBits;
    uint32_t              mipmapPrecisionBits;
    uint32_t              maxDrawIndexedIndexValue;
    uint32_t              maxDrawIndirectCount; */
	public var maxSamplerLodBias : hl.F32;
	public var maxSamplerAnisotropy : hl.F32;
	public var maxViewports : haxe.Int32;
//	public var maxViewportDimensions[2] : haxe.Int32;
//	public var viewportBoundsRange[2] : hl.F32;
	public var viewportSubPixelBits : haxe.Int32;
	public var minMemoryMapAlignment : Size_t;
	public var minTexelBufferOffsetAlignment : VkDeviceSize;
	public var minUniformBufferOffsetAlignment :VkDeviceSize;
	public var minStorageBufferOffsetAlignment : VkDeviceSize;
	public var minTexelOffset : haxe.Int32;
	public var maxTexelOffset : haxe.Int32;
	public var minTexelGatherOffset : haxe.Int32;
	public var maxTexelGatherOffset : haxe.Int32;
	public var minInterpolationOffset : hl.F32;
	public var maxInterpolationOffset : hl.F32;
	public var subPixelInterpolationOffsetBits : haxe.Int32;
	public var maxFramebufferWidth : haxe.Int32;
	public var maxFramebufferHeight : haxe.Int32;
	public var maxFramebufferLayers : haxe.Int32;
	public var framebufferColorSampleCounts : VkSampleCountFlags;
	public var framebufferDepthSampleCounts : VkSampleCountFlags;
	public var framebufferStencilSampleCounts : VkSampleCountFlags;
	public var framebufferNoAttachmentsSampleCounts : VkSampleCountFlags;
	public var maxColorAttachments : haxe.Int32;
	public var sampledImageColorSampleCounts : VkSampleCountFlags;
	public var sampledImageIntegerSampleCounts : VkSampleCountFlags;
	public var sampledImageDepthSampleCounts : VkSampleCountFlags;
	public var sampledImageStencilSampleCounts : VkSampleCountFlags;
	public var storageImageSampleCounts : VkSampleCountFlags;
	public var maxSampleMaskWords : haxe.Int32;
	public var timestampComputeAndGraphics : VkBool32;
	public var timestampPeriod : hl.F32;
	public var maxClipDistances : haxe.Int32;
	public var maxCullDistances : haxe.Int32;
	public var maxCombinedClipAndCullDistances : haxe.Int32;
	public var discreteQueuePriorities : haxe.Int32;
//	public var pointSizeRange[2] : hl.F32;
//	public var lineWidthRange[2] : hl.F32;
	public var pointSizeGranularity : hl.F32;
	public var lineWidthGranularity : hl.F32;
	public var strictLines : VkBool32;
	public var standardSampleLocations : VkBool32;
	public var optimalBufferCopyOffsetAlignment : VkDeviceSize;
	public var optimalBufferCopyRowPitchAlignment : VkDeviceSize;
	public var nonCoherentAtomSize : VkDeviceSize;
}

private class VkPhysicalDeviceSparseProperties { 
	public var residencyStandard2DBlockShape : VkBool32; 
	public var residencyStandard2DMultisampleBlockShape : VkBool32;
	public var residencyStandard3DBlockShape : VkBool32; 
	public var residencyAlignedMipSize : VkBool32;
	public var residencyNonResidentStrict : VkBool32;
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

private class VkExtent3D {
	public var width : haxe.Int32;
	public var height : haxe.Int32;
	public var depth : haxe.Int32;
}

private class VkQueueFamilyProperties 
{
	public var queueFlags : VkQueueFlags;
	public var queueCount : haxe.Int32;
	public var timestampValidBits : haxe.Int32;
	public var minImageTransferGranularity : VkExtent3D;
}

@:hlNative("vulkan")
class VkPhysicalDevice {


	private static var VK_QUEUE_GRAPHICS_BIT = 0x00000001;
	private static var VK_QUEUE_COMPUTE_BIT = 0x00000002;
	private static var VK_QUEUE_TRANSFER_BIT = 0x00000004;
	private static var VK_QUEUE_SPARSE_BINDING_BIT = 0x00000008;
	private static var VK_QUEUE_PROTECTED_BIT = 0x00000010;

	public static var VK_UUID_SIZE : Int = 16;
	public static var VK_MAX_PHYSICAL_DEVICE_NAME_SIZE : Int = 256;

	var ptr : VkPhysicalDevicePtr;
	var properties : VkPhysicalDeviceProperties;
	var queueFamilyProperties : Array<VkQueueFamilyProperties>;
	var apiVersion : Int;
	var driverVersion : Int;
	var vendorID : Int;
	var deviceID : Int;
	var deviceType : VkPhysicalDeviceType;
	var deviceName : String;
	var pipelineCacheUUID : String;

	public function getPtr() : VkPhysicalDevicePtr {
		return ptr;
	}
	
	public function new( p : VkPhysicalDevicePtr ) {
		// Store pointer to object
		ptr = p;

		// Retrieve physical device properties
		properties = new VkPhysicalDeviceProperties();
		vkGetPhysicalDeviceProperties(ptr, properties);
		retrieveProperties();

		// Retrieve queue family properties
		enumerateQueueFamilyProperties();

		// Print properties and QFP
		printProperties();
		var i : Int = 0;
		for (qfp in queueFamilyProperties) {
			trace("Queue " + i);
			printQueueFamilyProperties(qfp);
			i++;
		}
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

	public function printQueueFamilyProperties(p : VkQueueFamilyProperties) {
		var flags : Int = p.queueFlags;
		var flagsString : String = "";
		if ((flags & VK_QUEUE_GRAPHICS_BIT)!=0) flagsString += "VK_QUEUE_GRAPHICS_BIT";
		if ((flags & VK_QUEUE_COMPUTE_BIT)!=0) flagsString += " | VK_QUEUE_COMPUTE_BIT";
		if ((flags & VK_QUEUE_TRANSFER_BIT)!=0) flagsString += " | VK_QUEUE_TRANSFER_BIT";
		if ((flags & VK_QUEUE_SPARSE_BINDING_BIT)!=0) flagsString += " | VK_QUEUE_SPARSE_BINDING_BIT";
		if ((flags & VK_QUEUE_PROTECTED_BIT)!=0) flagsString += " | VK_QUEUE_PROTECTED_BIT";
		trace("queueFlags = " + flagsString);
		trace("queueCount = " + p.queueCount);
		trace("timestampValidBits = " + p.timestampValidBits);
		trace("minImageTransferGranularity.width = " + p.minImageTransferGranularity.width);
		trace("minImageTransferGranularity.height = " + p.minImageTransferGranularity.height);
		trace("minImageTransferGranularity.depth = " + p.minImageTransferGranularity.depth);
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

	public function enumerateQueueFamilyProperties() {
		queueFamilyProperties = new Array<VkQueueFamilyProperties>();
		var p : VkQueueFamilyProperties;
		do {
			p = vkGetPhysicalDeviceQueueFamilyPropertiesNext(ptr);
			if (p != null)
				queueFamilyProperties.push(p);
		} while (p != null);
	}
	
	@:hlNative("vulkan","vk_enumerate_physical_device_next")
	private static function vkEnumeratePhysicalDeviceNext( instancePtr : VkInstancePtr ) : VkPhysicalDevicePtr {
		return null;
	}

	@:hlNative("vulkan","vk_get_physical_device_properties")
	private static function vkGetPhysicalDeviceProperties( devicePtr : VkPhysicalDevicePtr, properties : VkPhysicalDeviceProperties) : Void {
	}

	@:hlNative("vulkan","vk_get_physical_device_queue_family_properties_next")
	private static function vkGetPhysicalDeviceQueueFamilyPropertiesNext( devicePtr : VkPhysicalDevicePtr ) : VkQueueFamilyProperties {
		return null;
	}
}
