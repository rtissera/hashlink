#define HL_NAME(n) vulkan_##n

#include <hl.h>
#include <assert.h>

//hack
#define HL_VULKAN

#ifdef HL_VULKAN

#if defined(HL_IOS) || defined (HL_TVOS) || defined(HL_MAC)
//#	error "No Vulkan support (use MoltenVK)"
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_syswm.h>
#	include <SDL2/SDL_vulkan.h>
#	include <vulkan/vulkan.h>
#elif defined(_WIN32)
#	include <SDL.h>
#	include <SDL_vulkan.h>
#	include <vulkan/vulkan.h>
#elif defined(HL_CONSOLE)
#	include <vulkan/vulkan.h>
#elif defined(HL_MESA)
#	error "No Vulkan support for HL_MESA"
#elif defined(HL_ANDROID)
#	include <SDL.h>
#	include <SDL_vulkan.h>
#	include <vulkan/vulkan.h>
#else
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_vulkan.h>
#	include <vulkan/vulkan.h>
#endif

#ifndef HL_VULKAN_APP_NAME
#define HL_VULKAN_APP_NAME "HashLink Vulkan App"
#endif

#ifndef HL_VULKAN_ENGINE_NAME
#define HL_VULKAN_ENGINE_NAME "HashLink Vulkan Engine"
#endif

#ifndef VK_ALLOC
#define VK_ALLOC hl_alloc_bytes
#endif

// Note : Might need to deal with vkGetInstanceProcAddr ?

static int VKLoadAPI() {
	// Load vulkan library
	int result = SDL_Vulkan_LoadLibrary(NULL);
	if (result != 0)
		return 1;

	// Everything is okay
	return 0;
}

// helpers to store and marshall Vulkan structures

static vdynamic *alloc_i32(int v) {
	vdynamic *ret;
	ret = hl_alloc_dynamic(&hlt_i32);
	ret->v.i = v;
	return ret;
}

static vdynamic *alloc_ptr(void* ptr) {
	vdynamic *ret;
	ret = hl_alloc_dynamic(&hlt_dyn);
	ret->v.ptr = ptr;
	return ret;
}

#define ZIDX(val) ((val)?(val)->v.i:0)
#define ZPTR(val) ((val)?(val)->v.ptr:NULL)


// Used for validating return values of Vulkan API calls.
#define VK_CHECK_RESULT(f) 																	\
{																							\
    VkResult res = (f);																		\
    if (res != VK_SUCCESS)																	\
    {																						\
        hl_error("Fatal : VkResult is %d in %s at line %d\n", res,  __FILE__, __LINE__);	\
        assert(res == VK_SUCCESS);															\
    }																						\
}

// globals
HL_PRIM bool HL_NAME(vk_init)() {
	return VKLoadAPI() == 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Vulkan Types
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define TVKINSTANCE		_ABSTRACT(vk_instance)
#define TVKPHYSICALDEVICE	_ABSTRACT(vk_physical_device)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Instances API
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// vkCreateInstance
HL_PRIM VkInstance* HL_NAME(vk_create_instance)() {
	VkApplicationInfo appInfo;
	VkInstanceCreateInfo createInfo;
	unsigned int extensionCount = 0;
	VkExtensionProperties* extensions = NULL;
	VkInstance* instance = NULL;

	// Fill app info
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = HL_VULKAN_APP_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = HL_VULKAN_ENGINE_NAME;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Fill create info
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// TODO add support for validation layers
	createInfo.enabledLayerCount = 0;

	// Enumerate available extensions
	if (!SDL_Vulkan_GetInstanceExtensions(NULL,  &extensionCount, NULL)) {
		hl_error("Fatal : SDL_Vulkan_GetInstanceExtensions (1) error\n");
		return instance;
	}

	const char **extensionNames = malloc(sizeof(const char *) * extensionCount);
	if(!extensionNames) {
		hl_error("Fatal : Cannot allocate extensionNames\n");
		return instance;
	}

	if(!SDL_Vulkan_GetInstanceExtensions(NULL, &extensionCount, extensionNames)) {
		hl_error("Fatal : SDL_Vulkan_GetInstanceExtensions (2) error\n");
		for (int i = 0 ; i < extensionCount ; i++)
			free((void*)extensionNames[i]);
		free(extensionNames);
		return instance;
	}

	// Now we can finish filling VkInstanceCreateInfo structure
	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensionNames;

	// TODO custom memory allocator ??
	// VkAllocationCallbacks allocator;

	instance = malloc(sizeof(VkInstance));
	VK_CHECK_RESULT(vkCreateInstance(&createInfo, NULL/*&allocator*/, instance));

	// Free memory
	for (int i = 0 ; i < extensionCount ; i++)
		free((void*)extensionNames[i]);
	free(extensionNames);

	// Return VkInstance object
	return instance;
}

HL_PRIM void HL_NAME(vk_destroy_instance)(VkInstance* pInstance) {
	if (pInstance != NULL) {
		vkDestroyInstance(*pInstance, NULL);
		free(pInstance); // TODO CHECK THAT (Not sure...)
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Physical devices API
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// vkEnumeratePhysicalDevices
HL_PRIM varray *HL_NAME(vk_enumerate_physical_devices)(vdynamic* i) {
	unsigned int deviceCount = 0;
	VkPhysicalDevice* devices = NULL;
	VkInstance* pInstance = (VkInstance*)i->v.ptr;
	VkInstance instance = *pInstance;

	// Retrieve number of physical devices
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &deviceCount, NULL));
	if (deviceCount == 0) {
		hl_error("Fatal : vkEnumeratePhysicalDevices failed to find GPUs with Vulkan support!\n");
		return NULL;
	}

	// Retrieve list of physical devices
	devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &deviceCount, devices));

	// Marshall to VM
	varray* aDevices = hl_alloc_array(&hlt_dyn, deviceCount);
	for (int i = 0 ; i < deviceCount ; i++) {
		hl_aptr(aDevices,vdynamic*)[i] = alloc_ptr(&devices[i]);
	}

	// Return VkPhysicalDevice list
	return aDevices;
}

// vkGetPhysicalDeviceProperties
HL_PRIM vdynamic *HL_NAME(vk_get_physical_device_properties)( vdynamic *physicalDevice ) {
	VkPhysicalDeviceProperties* properties = malloc(sizeof(VkPhysicalDeviceProperties));
	vkGetPhysicalDeviceProperties(physicalDevice->v.ptr, properties);
	return alloc_ptr(properties);
}

// vkGetPhysicalDeviceQueueFamilyProperties
HL_PRIM varray *HL_NAME(vk_get_physical_device_queue_family_properties)( vdynamic *physicalDevice) {
	unsigned int queueFamilyPropertyCount;
	VkQueueFamilyProperties* queueFamilyProperties = NULL;

	// Get count
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->v.ptr, &queueFamilyPropertyCount, queueFamilyProperties);

	if (queueFamilyPropertyCount > 0)
	{
		// Alloc 
		queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyPropertyCount);

		// Query 
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->v.ptr, &queueFamilyPropertyCount, queueFamilyProperties);

		// Copy
		varray* aQueueFamilyProperties = hl_alloc_array(&hlt_dyn, queueFamilyPropertyCount);
		for (int i = 0 ; i < queueFamilyPropertyCount ; i++) {
			hl_aptr(aQueueFamilyProperties,vdynamic*)[i] = alloc_ptr(&queueFamilyProperties[i]);
		}

		return aQueueFamilyProperties;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Devices API
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HL_PRIM vdynamic *HL_NAME(vk_create_device)( vdynamic *physicalDevice ) {
	VkDeviceCreateInfo deviceCreateInfo;
	VkDevice* device = malloc(sizeof(VkDevice));
	// TODO fill VkDeviceCreateInfo structure

	VK_CHECK_RESULT(vkCreateDevice(physicalDevice->v.ptr, &deviceCreateInfo, NULL, device));
	return alloc_ptr(device);
}

HL_PRIM vdynamic *HL_NAME(vk_device_wait_idle)( vdynamic *device ) {
	VkResult result;
	result = vkDeviceWaitIdle(device->v.ptr);
	return alloc_i32(result);
}

HL_PRIM void HL_NAME(vk_destroy_device)( vdynamic *device ) {
	vkDestroyDevice(device->v.ptr, NULL);
}

// Queues API

HL_PRIM vdynamic *HL_NAME(vk_get_device_queue)( vdynamic *device, int queueFamilyIndex, int queueIndex) {
	VkQueue queue;
	vkGetDeviceQueue(device->v.ptr, queueFamilyIndex, queueIndex, &queue);
	return alloc_ptr(queue);
}

HL_PRIM vdynamic *HL_NAME(vk_queue_wait_idle)( vdynamic *queue ) {
	VkResult result = vkQueueWaitIdle(queue->v.ptr);
	return alloc_i32(result);
}

// Command Buffers API

/*HL_PRIM vdynamic *HL_NAME(vk_create_command_pool)( vdynamic *device, )

HL_PRIM void HL_NAME(gl_link_program)( vdynamic *p ) {
	glLinkProgram(p->v.i);
}

HL_PRIM vdynamic *HL_NAME(gl_get_program_parameter)( vdynamic *p, int param ) {
	switch( param ) {
	case 0x8B82 : {
		int ret = 0;
		glGetProgramiv(p->v.i, param, &ret);
		return alloc_i32(ret);
	}
	default:
		hl_error("Unsupported param %d",param);
	}
	return NULL;
}

HL_PRIM vbyte *HL_NAME(gl_get_program_info_bytes)( vdynamic *p ) {
	char log[4096];
	*log = 0;
	glGetProgramInfoLog(p->v.i, 4096, NULL, log);
	return hl_copy_bytes((vbyte*)log,(int)strlen(log) + 1);
}

HL_PRIM vdynamic *HL_NAME(gl_get_uniform_location)( vdynamic *p, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
	int u = glGetUniformLocation(p->v.i, cname);
	if( u < 0 ) return NULL;
	return alloc_i32(u);
}

HL_PRIM int HL_NAME(gl_get_attrib_location)( vdynamic *p, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
	return glGetAttribLocation(p->v.i, cname);
}

HL_PRIM void HL_NAME(gl_use_program)( vdynamic *p ) {
	glUseProgram(ZIDX(p));
}

// shader

HL_PRIM vdynamic *HL_NAME(gl_create_shader)( int type ) {
	int s = glCreateShader(type);
	if (s == 0) return NULL;
	return alloc_i32(s);
}

HL_PRIM void HL_NAME(gl_shader_source)( vdynamic *s, vstring *src ) {
	const GLchar *c = (GLchar*)hl_to_utf8(src->bytes);
	glShaderSource(s->v.i, 1, &c, NULL);
}

HL_PRIM void HL_NAME(gl_compile_shader)( vdynamic *s ) {
	glCompileShader(s->v.i);
}

HL_PRIM vbyte *HL_NAME(gl_get_shader_info_bytes)( vdynamic *s ) {
	char log[4096];
	*log = 0;
	glGetShaderInfoLog(s->v.i, 4096, NULL, log);
	return hl_copy_bytes((vbyte*)log, (int)strlen(log)+1);
}

HL_PRIM vdynamic *HL_NAME(gl_get_shader_parameter)( vdynamic *s, int param ) {
	switch( param ) {
	case 0x8B81:
	case 0x8B4F:
	case 0x8B80:
	{
		int ret = 0;
		glGetShaderiv(s->v.i, param, &ret);
		return alloc_i32(ret);
	}
	default:
		hl_error("Unsupported param %d", param);
	}
	return NULL;
}

HL_PRIM void HL_NAME(gl_delete_shader)( vdynamic *s ) {
	glDeleteShader(s->v.i);
}

// texture

HL_PRIM vdynamic *HL_NAME(gl_create_texture)() {
	unsigned int t = 0;
	glGenTextures(1, &t);
	return alloc_i32(t);
}

HL_PRIM void HL_NAME(gl_active_texture)( int t ) {
	glActiveTexture(t);
}

HL_PRIM void HL_NAME(gl_bind_texture)( int t, vdynamic *texture ) {
	glBindTexture(t, ZIDX(texture));
}

HL_PRIM void HL_NAME(gl_bind_image_texture)( int unit, int texture, int level, bool layered, int layer, int access, int format ) {
#	if !defined(HL_IOS) && !defined(HL_TVOS) && !defined(HL_MAC) && !defined(HL_MESA)
	glBindImageTexture(unit, texture, level, layered, layer, access, format);
#	endif
}

HL_PRIM void HL_NAME(gl_tex_image2d_multisample)( int target, int samples, int internalFormat, int width, int height, bool fixedsamplelocations) {
#	if !defined(HL_MESA)
	glTexImage2DMultisample(target, samples, internalFormat, width, height, fixedsamplelocations);
#	endif
}

HL_PRIM void HL_NAME(gl_generate_mipmap)( int t ) {
	glGenerateMipmap(t);
}

HL_PRIM void HL_NAME(gl_delete_texture)( vdynamic *t ) {
	unsigned int tt = t->v.i;
	glDeleteTextures(1, &tt);
}

// framebuffer

HL_PRIM void HL_NAME(gl_blit_framebuffer)(int src_x0, int src_y0, int src_x1, int src_y1, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int mask, int filter) {
	glBlitFramebuffer(src_x0, src_y0, src_x1, src_y1, dst_x0, dst_y0, dst_x1, dst_y1, mask, filter);
}

HL_PRIM vdynamic *HL_NAME(gl_create_framebuffer)() {
	unsigned int f = 0;
	glGenFramebuffers(1, &f);
	return alloc_i32(f);
}

HL_PRIM void HL_NAME(gl_bind_framebuffer)( int target, vdynamic *f ) {
	unsigned int id = ZIDX(f);
#if	defined(HL_IOS) || defined(HL_TVOS)
	if ( id==0 ) {
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(SDL_GL_GetCurrentWindow(), &info);
		id = info.info.uikit.framebuffer;
	}
#endif
	glBindFramebuffer(target, id);
}

HL_PRIM void HL_NAME(gl_framebuffer_texture)( int target, int attach, vdynamic *t, int level ) {
#if !defined (HL_MESA)
	glFramebufferTexture(target, attach, ZIDX(t), level);
#endif
}

// renderbuffer

HL_PRIM vdynamic *HL_NAME(gl_create_renderbuffer)() {
	unsigned int buf = 0;
	glGenRenderbuffers(1, &buf);
	return alloc_i32(buf);
}

HL_PRIM void HL_NAME(gl_bind_renderbuffer)( int target, vdynamic *r ) {
	unsigned int id = ZIDX(r);
#if	defined(HL_IOS) || defined(HL_TVOS)
	if ( id==0 ) {
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(SDL_GL_GetCurrentWindow(), &info);
		id = info.info.uikit.colorbuffer;
	}
#endif
	glBindRenderbuffer(GL_RENDERBUFFER, id);
}

HL_PRIM void HL_NAME(gl_renderbuffer_storage)( int target, int format, int width, int height ) {
	glRenderbufferStorage(target, format, width, height);
}

// buffer

HL_PRIM vdynamic *HL_NAME(gl_create_buffer)() {
	unsigned int b = 0;
	glGenBuffers(1, &b);
	return alloc_i32(b);
}

HL_PRIM void HL_NAME(gl_delete_buffer)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	glDeleteBuffers(1, &bb);
}

// uniforms

HL_PRIM void HL_NAME(gl_uniform1i)( vdynamic *u, int i ) {
	glUniform1i(u->v.i, i);
}

HL_PRIM void HL_NAME(gl_uniform4fv)( vdynamic *u, vbyte *buffer, int bufPos, int count ) {
	glUniform4fv(u->v.i, count, (float*)buffer + bufPos);
}

HL_PRIM void HL_NAME(gl_uniform_matrix4fv)( vdynamic *u, bool transpose, vbyte *buffer, int bufPos, int count ) {
	glUniformMatrix4fv(u->v.i, count, transpose ? GL_TRUE : GL_FALSE, (float*)buffer + bufPos);
}

// compute
HL_PRIM void HL_NAME(gl_dispatch_compute)( int num_groups_x, int num_groups_y, int num_groups_z ) {
#	if !defined(HL_IOS) && !defined(HL_TVOS) && !defined(HL_MAC) && !defined(HL_MESA)
	glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
#	endif
}

HL_PRIM void HL_NAME(gl_memory_barrier)( int barriers ) {
#	if !defined(HL_IOS) && !defined(HL_TVOS) && !defined(HL_MAC) && !defined(HL_MESA)
	glMemoryBarrier(barriers);
#	endif
}

// draw

HL_PRIM void HL_NAME(gl_draw_elements)( int mode, int count, int type, int start ) {
	glDrawElements(mode, count, type, (void*)(int_val)start);
}

HL_PRIM void HL_NAME(gl_draw_arrays)( int mode, int first, int count, int start ) {
	glDrawArrays(mode,first,count);
}

HL_PRIM void HL_NAME(gl_draw_elements_instanced)( int mode, int count, int type, int start, int primcount ) {
	glDrawElementsInstanced(mode,count,type,(void*)(int_val)start,primcount);
}

HL_PRIM void HL_NAME(gl_draw_arrays_instanced)( int mode, int first, int count, int primcount ) {
	glDrawArraysInstanced(mode,first,count,primcount);
}

HL_PRIM void HL_NAME(gl_multi_draw_elements_indirect)( int mode, int type, vbyte *data, int count, int stride ) {
#	ifdef GL_VERSION_4_3
	glMultiDrawElementsIndirect(mode, type, data, count, stride);
#	endif
}

HL_PRIM int HL_NAME(gl_get_config_parameter)( int feature ) {
	switch( feature ) {
	case 0:
#		ifdef GL_VERSION_4_3
		return 1;
#		else
		return 0;
#		endif
	default:
		break;
	}
	return -1;
}

// queries

HL_PRIM vdynamic *HL_NAME(gl_create_query)() {
	unsigned int t = 0;
	glGenQueries(1, &t);
	return alloc_i32(t);
}

HL_PRIM void HL_NAME(gl_delete_query)( vdynamic *q ) {
	glDeleteQueries(1, (const GLuint *) &q->v.i);
}

HL_PRIM void HL_NAME(gl_begin_query)( int target, vdynamic *q ) {
	glBeginQuery(target,q->v.i);
}

HL_PRIM void HL_NAME(gl_end_query)( int target ) {
	glEndQuery(target);
}

HL_PRIM bool HL_NAME(gl_query_result_available)( vdynamic *q ) {
	int v = 0;
	glGetQueryObjectiv(q->v.i, GL_QUERY_RESULT_AVAILABLE, &v);
	return v == GL_TRUE;
}

HL_PRIM double HL_NAME(gl_query_result)( vdynamic *q ) {
	GLuint64 v = -1;
#	if !defined(HL_MESA) && !defined(HL_MOBILE)
	glGetQueryObjectui64v(q->v.i, GL_QUERY_RESULT, &v);
#	endif
	return (double)v;
}

HL_PRIM void HL_NAME(gl_query_counter)( vdynamic *q, int target ) {
#	if !defined(HL_MESA) && !defined(HL_MOBILE)
	glQueryCounter(q->v.i, target);
#	endif
}

// vertex array

HL_PRIM vdynamic *HL_NAME(gl_create_vertex_array)() {
	unsigned int f = 0;
	glGenVertexArrays(1, &f);
	return alloc_i32(f);
}

HL_PRIM void HL_NAME(gl_bind_vertex_array)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	glBindVertexArray(bb);
}

HL_PRIM void HL_NAME(gl_delete_vertex_array)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	glDeleteVertexArrays(1, &bb);
}

// uniform buffer

HL_PRIM int HL_NAME(gl_get_uniform_block_index)( vdynamic *p, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
	return (int)glGetUniformBlockIndex(p->v.i, cname);
}

HL_PRIM void HL_NAME(gl_uniform_block_binding)( vdynamic *p, int index, int binding ) {
	glUniformBlockBinding(p->v.i, index, binding);
}

DEFINE_PRIM(_BOOL,gl_init,_NO_ARG);
DEFINE_PRIM(_BOOL,gl_is_context_lost,_NO_ARG);
DEFINE_PRIM(_VOID,gl_clear,_I32);
DEFINE_PRIM(_I32,gl_get_error,_NO_ARG);
DEFINE_PRIM(_VOID,gl_scissor,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_clear_color,_F64 _F64 _F64 _F64);
DEFINE_PRIM(_VOID,gl_clear_depth,_F64);
DEFINE_PRIM(_VOID,gl_clear_stencil,_I32);
DEFINE_PRIM(_VOID,gl_viewport,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_finish,_NO_ARG);
DEFINE_PRIM(_VOID,gl_flush,_NO_ARG);
DEFINE_PRIM(_VOID,gl_pixel_storei,_I32 _I32);
DEFINE_PRIM(_BYTES,gl_get_string,_I32);
DEFINE_PRIM(_VOID,gl_polygon_mode,_I32 _I32);
DEFINE_PRIM(_VOID,gl_enable,_I32);
DEFINE_PRIM(_VOID,gl_disable,_I32);
DEFINE_PRIM(_VOID,gl_cull_face,_I32);
DEFINE_PRIM(_VOID,gl_blend_func,_I32 _I32);
DEFINE_PRIM(_VOID,gl_blend_func_separate,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_blend_equation,_I32);
DEFINE_PRIM(_VOID,gl_blend_equation_separate,_I32 _I32);
DEFINE_PRIM(_VOID,gl_depth_mask,_BOOL);
DEFINE_PRIM(_VOID,gl_depth_func,_I32);
DEFINE_PRIM(_VOID,gl_color_mask,_BOOL _BOOL _BOOL _BOOL);
DEFINE_PRIM(_VOID,gl_stencil_mask_separate,_I32 _I32);
DEFINE_PRIM(_VOID,gl_stencil_func_separate,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_stencil_op_separate,_I32  _I32 _I32 _I32);
DEFINE_PRIM(_NULL(_I32),gl_create_program,_NO_ARG);
DEFINE_PRIM(_VOID,gl_delete_program,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_bind_frag_data_location,_NULL(_I32) _I32 _STRING);
DEFINE_PRIM(_VOID,gl_attach_shader,_NULL(_I32) _NULL(_I32));
DEFINE_PRIM(_VOID,gl_link_program,_NULL(_I32));
DEFINE_PRIM(_DYN,gl_get_program_parameter,_NULL(_I32) _I32);
DEFINE_PRIM(_BYTES,gl_get_program_info_bytes,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_get_uniform_location,_NULL(_I32) _STRING);
DEFINE_PRIM(_I32,gl_get_attrib_location,_NULL(_I32) _STRING);
DEFINE_PRIM(_VOID,gl_use_program,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_create_shader,_I32);
DEFINE_PRIM(_VOID,gl_shader_source,_NULL(_I32) _STRING);
DEFINE_PRIM(_VOID,gl_compile_shader,_NULL(_I32));
DEFINE_PRIM(_BYTES,gl_get_shader_info_bytes,_NULL(_I32));
DEFINE_PRIM(_DYN,gl_get_shader_parameter,_NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_delete_shader,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_create_texture,_NO_ARG);
DEFINE_PRIM(_VOID,gl_active_texture,_I32);
DEFINE_PRIM(_VOID,gl_bind_texture,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_tex_parameteri,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_tex_parameterf,_I32 _I32 _F32);
DEFINE_PRIM(_VOID,gl_tex_image2d,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_tex_image3d,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_tex_image2d_multisample,_I32 _I32 _I32 _I32 _I32 _BOOL);
DEFINE_PRIM(_VOID,gl_generate_mipmap,_I32);
DEFINE_PRIM(_VOID,gl_delete_texture,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_blit_framebuffer,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32);
DEFINE_PRIM(_NULL(_I32),gl_create_framebuffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_framebuffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_framebuffer_texture,_I32 _I32 _NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_framebuffer_texture2d,_I32 _I32 _I32 _NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_framebuffer_texture_layer,_I32 _I32 _NULL(_I32) _I32 _I32);
DEFINE_PRIM(_VOID,gl_delete_framebuffer,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_read_pixels,_I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_read_buffer,_I32);
DEFINE_PRIM(_VOID,gl_draw_buffers,_I32 _BYTES);
DEFINE_PRIM(_NULL(_I32),gl_create_renderbuffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_renderbuffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_renderbuffer_storage,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_renderbuffer_storage_multisample,_I32 _I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_framebuffer_renderbuffer,_I32 _I32 _I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_delete_renderbuffer,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_create_buffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_buffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_bind_buffer_base,_I32 _I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_buffer_data_size,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_buffer_data,_I32 _I32 _BYTES _I32);
DEFINE_PRIM(_VOID,gl_buffer_sub_data,_I32 _I32 _BYTES _I32 _I32);
DEFINE_PRIM(_VOID,gl_enable_vertex_attrib_array,_I32);
DEFINE_PRIM(_VOID,gl_disable_vertex_attrib_array,_I32);
DEFINE_PRIM(_VOID,gl_vertex_attrib_pointer,_I32 _I32 _I32 _BOOL _I32 _I32);
DEFINE_PRIM(_VOID,gl_vertex_attrib_ipointer,_I32 _I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_delete_buffer,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_uniform1i,_NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_uniform4fv,_NULL(_I32) _BYTES _I32 _I32);
DEFINE_PRIM(_VOID,gl_uniform_matrix4fv,_NULL(_I32) _BOOL _BYTES _I32 _I32);
DEFINE_PRIM(_VOID,gl_bind_image_texture,_I32 _I32 _I32 _BOOL _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_dispatch_compute,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_memory_barrier,_I32);
DEFINE_PRIM(_VOID,gl_draw_elements,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_draw_elements_instanced,_I32 _I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_draw_arrays,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_draw_arrays_instanced,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_multi_draw_elements_indirect, _I32 _I32 _BYTES _I32 _I32);
DEFINE_PRIM(_NULL(_I32),gl_create_vertex_array,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_vertex_array,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_delete_vertex_array,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_vertex_attrib_divisor,_I32 _I32);

DEFINE_PRIM(_NULL(_I32), gl_create_query, _NO_ARG);
DEFINE_PRIM(_VOID, gl_delete_query, _NULL(_I32));
DEFINE_PRIM(_VOID, gl_begin_query, _I32 _NULL(_I32));
DEFINE_PRIM(_VOID, gl_end_query, _I32);
DEFINE_PRIM(_BOOL, gl_query_result_available, _NULL(_I32));
DEFINE_PRIM(_VOID, gl_query_counter, _NULL(_I32) _I32);
DEFINE_PRIM(_F64, gl_query_result, _NULL(_I32));

DEFINE_PRIM(_I32, gl_get_uniform_block_index, _NULL(_I32) _STRING);
DEFINE_PRIM(_VOID, gl_uniform_block_binding, _NULL(_I32) _I32 _I32);

DEFINE_PRIM(_I32, gl_get_config_parameter, _I32);
*/

DEFINE_PRIM(_BOOL, vk_init,_NO_ARG);
DEFINE_PRIM(TVKINSTANCE, vk_create_instance, _NO_ARG);
DEFINE_PRIM(_VOID, vk_destroy_instance, TVKINSTANCE);

DEFINE_PRIM(_ARR, vk_enumerate_physical_devices, TVKINSTANCE);

#endif
