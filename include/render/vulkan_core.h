#pragma once
#include "render/vulkan.h"
#include "render/vulkan_create_infos.h"

#define VULKAN_VALIDATION_LAYER "VK_LAYER_KHRONOS_validation"

namespace as
{
	namespace vk
	{
		/** Instance */
		VkResult create_vulkan_instance(VkInstance& out_instance, const instance_create_info& instance_create_info);

		/** Surface */
		VkResult create_surface(VkSurfaceKHR& out_surface, const VkInstance& instance, GLFWwindow* window);
		VkResult create_surface(const surface_create_info& create_info, VkSurfaceKHR& out_surface);

		/** Physical device */
		//QueueFamilyIndices find_queue_families(VkPhysicalDevice& physical_device, VkSurfaceKHR* surface);
		QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR* surface);
		bool check_device_extension_support(VkPhysicalDevice& physical_device, const std::vector<const char*> extensions);
		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice physical_device, VkSurfaceKHR* surface);
		bool is_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR* surface);
		VkSampleCountFlagBits get_max_usable_sample_count(VkPhysicalDevice* physical_device);
		void pick_physical_device(VkPhysicalDevice* out_physical_device, VkSampleCountFlagBits* out_msaa_samples, VkInstance* instance, VkSurfaceKHR* surface);
		bool pick_physical_device(const physical_device_create_info& create_info, VkPhysicalDevice& out_physical_device, VkSampleCountFlagBits& out_msaa_samples);

		/** Logical device */
		//void create_logical_device(VkDevice* out_logical_device, VkQueue* out_graphics_queue, VkQueue* out_present_queue, VkPhysicalDevice* physical_device, VkSurfaceKHR* surface, const std::vector<const char*> extensions, const std::vector<const char*> validation_layers);
		void create_logical_device(const logical_device_create_info& create_info, VkDevice* out_logical_device, VkQueue* out_graphics_queue, VkQueue* out_present_queue);

		/** Swap chain */
		VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D choos_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
		void create_swap_chain(VkSwapchainKHR* out_swap_chain, std::vector<VkImage>* out_swap_chain_images, VkFormat* out_swap_chain_image_format, VkExtent2D* out_swap_chain_extent, VkDevice* logical_device, VkPhysicalDevice* physical_device, VkSurfaceKHR* surface, GLFWwindow* window);
		void create_swapchain(const swapchain_create_info& create_info, as::vk::swapchain& out_swapchain);
		void cleanup_swap_chain(VkDevice& logical_device, VkSwapchainKHR& swap_chain, std::vector<image_data>& images_data, std::vector<VkFramebuffer>& frame_buffers, std::vector<VkImageView>& swap_chain_image_views);

		/** Image */
		void create_image(VkPhysicalDevice& physical_device, VkDevice& logical_device, u32 width, u32 height, u32 mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkResult create_image(const image_create_info& create_info, image_data& out_image_data);
		u32 find_memory_type(VkPhysicalDevice physical_device, u32 typeFilter, VkMemoryPropertyFlags properties);
		void create_color_resources(VkImageView& out_image_view, VkPhysicalDevice& physical_device, VkDevice& logical_device, VkImage& image, VkDeviceMemory& image_memory, VkFormat& swap_chain_image_format, VkExtent2D& swap_chain_extent, VkSampleCountFlagBits& msaa_samples);
		void create_depth_resources(VkImageView& out_image_view, VkPhysicalDevice& physical_device, VkDevice& logical_device, VkImage& image, VkDeviceMemory& image_memory, VkFormat& swap_chain_image_format, VkExtent2D& swap_chain_extent, VkSampleCountFlagBits& msaa_samples);

		/** Image view */
		VkImageView create_image_view(VkDevice* logical_device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);
		VkResult create_image_view(const image_view_create_info& create_info, VkImageView* image_view);
		void create_image_views(std::vector<VkImageView>* swap_chain_image_views, std::vector<VkFramebuffer>* swap_chain_framebuffers, std::vector<VkImage>* swap_chain_images, VkFormat* swap_chain_image_format, VkDevice* logical_device);
		VkResult create_render_pass(VkFormat& swap_chain_image_format, VkSampleCountFlagBits& msaa_samples, VkRenderPass& render_pass, VkDevice& logical_device, VkPhysicalDevice& physical_device);
		VkResult create_render_pass(const render_pass_create_info& create_info, VkRenderPass& out_render_pass);
		VkFormat find_supported_format(VkPhysicalDevice& physical_device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat find_depth_format(VkPhysicalDevice physical_device);

		/** Descriptor && Shader */
		VkResult create_descriptor_set_layout(VkDevice& logical_device, VkDescriptorSetLayout& out_descriptor_set_layout);
		VkResult create_pipeline(const pipeline_create_info& create_info, pipeline_data& out_pipeline);
		VkShaderModule create_shader_module(const std::vector<char>& code, VkDevice logical_device);
		VkResult create_command_pool(VkCommandPool& out_command_pool, VkPhysicalDevice& physical_device, const VkDevice& logical_device, VkSurfaceKHR& surface);

		/** Framebuffer */
		void create_frame_buffers(std::vector<VkFramebuffer>& out_swap_chain_framebuffers, VkDevice& logical_device, std::vector<VkImageView>& swap_chain_image_views, VkImageView& color_image_view, VkImageView& depth_image_view, VkRenderPass& render_pass, VkExtent2D& swap_chain_extent);

		/** Texture && Buffer && Layout */
		void create_texture_image(VkImage& out_texture_image, const char* texture_path, u32& mip_levels, VkPhysicalDevice& physical_device, VkDevice& logical_device, VkCommandPool& command_pool, VkQueue& graphics_queue, VkDeviceMemory& texture_image_memory);
		void create_texture_image_view(VkImageView& out_texture_image_view, VkDevice& logical_device, VkImage& image, const u32& mip_levels);
		VkResult create_texture_sampler(VkSampler& out_texture_sampler, VkPhysicalDevice& physical_device, VkDevice& logical_device, const u32& mip_levels);
		VkResult create_buffer(VkBuffer& out_buffer, VkPhysicalDevice& physical_device, VkDevice& logical_device, VkDeviceSize& size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& buffer_memory);
		void transition_image_layout(VkDevice& logical_device, VkCommandPool& command_pool, VkQueue& graphics_queue, VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, u32 mipLevels);
		VkCommandBuffer begin_single_time_commands(VkDevice& logical_device, VkCommandPool& command_pool);
		void end_single_time_commands(VkDevice& logical_device, VkCommandPool& command_pool, VkCommandBuffer command_buffer, VkQueue& graphics_queue); /** unsure if the command_buffer should be a copy or ref */
		void copy_buffer_to_image(VkDevice& logical_device, VkCommandPool& command_pool, VkQueue& graphics_queue, VkBuffer buffer, VkImage image, u32 width, u32 height);
		void generate_mipmaps(VkPhysicalDevice& physical_device, VkDevice& logical_device, VkCommandPool& command_pool, VkQueue& queue, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, u32 mip_levels);

		/** Model && Buffer */
		void load_model(const char* modle_path, std::vector<Vertex>& out_vertices, std::vector<u32>& out_indices);
		void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size, VkDevice& logical_device, VkCommandPool& command_pool, VkQueue& queue);
		void create_vertex_buffer(VkBuffer& out_vertex_buffer, VkDeviceMemory& vertex_buffer_memory, VkPhysicalDevice& physical_device, VkDevice& logical_device, const std::vector<Vertex>& vertices, VkCommandPool& command_pool, VkQueue& queue);
		void create_index_buffer(VkBuffer& out_index_buffer, VkDeviceMemory& index_buffer_memory, VkPhysicalDevice& physical_device, VkDevice& logical_device, const std::vector<u32>& indices, VkCommandPool& command_pool, VkQueue& queue);
		void create_uniform_buffers(std::vector<VkBuffer>& out_uniform_buffers, std::vector<VkDeviceMemory>& out_uniform_buffers_memory, VkPhysicalDevice& physical_device, VkDevice& logical_device, const i8& max_frames_in_flight);
		VkResult create_descriptor_pool(VkDescriptorPool& descriptor_tool, VkDevice& logical_device, const i8& max_frames_in_flight);
		VkResult create_descriptor_sets(std::vector<VkDescriptorSet>& out_descriptor_sets, VkDevice& logical_device, VkDescriptorSetLayout& descriptor_set_layout, VkDescriptorPool& descriptor_pool, const i8& max_frames_in_flight);;
		void update_descriptor_sets(VkDevice& logical_device, std::vector<VkDescriptorSet>& out_descriptor_sets, std::vector<VkBuffer>& uniform_buffers, const i8& max_frames_in_flight, VkImageView& image_view, VkSampler& image_sampler);;
		VkResult create_command_buffers(std::vector<VkCommandBuffer>& command_buffers, VkDevice& logical_device, VkCommandPool& command_pool, const i8& max_frames_in_flight);

		/** Synchronization */
		void create_sync_objects(VkDevice& logical_device, std::vector<VkSemaphore>& image_available_semaphores, std::vector<VkSemaphore>& render_finished_semaphores, std::vector<VkFence>& inflight_fences, const i8& max_frames_in_flight);

		/** Debug */
		VkResult setup_debug_messenger(VkInstance* instance, VkDebugUtilsMessengerEXT* debug_messenger);
		VkResult create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		/** Utility */
		bool check_layers_support(const std::vector<const char*> layers);
		std::vector<const char*> get_required_extensions(const bool& enable_validation_layers);

	};

	/** Files */
	std::vector<char> read_file(const std::string& filename);
	char* read_file(const char* filename);
	void write_file_str(const char* filename, const char* data);

};