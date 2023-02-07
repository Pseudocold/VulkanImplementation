#pragma once

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include<array>

#include "Mesh.h"
#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* newWindow);

	void updateModel(glm::mat4 newModel);

	void draw();
	void cleanup(); // whenever the vkCreate*() is called, there also needs a destroy function to be called in cleanup()

	~VulkanRenderer();

private:
	GLFWwindow* window;

	int currentFrame = 0;

	//Scene Objects
	std::vector<Mesh> meshList;

	//Scene Settings
	struct MVP {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
	}mvp;

	//Vulkan components
	// - Main
	VkInstance instance;
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;

	std::vector<SwapchainImage> swapChainImages;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;

	// - Descriptor
	VkDescriptorSetLayout descriptorSetLayout;		// how descripor be laid out on a shader

	VkDescriptorPool descriptorPool;						// where the descriptor sets will be allocated
	std::vector<VkDescriptorSet> descriptorSets;	// 

	std::vector<VkBuffer> uniformBuffer;					// the raw data that descriptor will point to and describe
	std::vector<VkDeviceMemory> uniformBufferMemory;

	// - Pipeline
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;


	// - Pools
	VkCommandPool graphicsCommandPool;


	// - Utility
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// - Synchronization
	std::vector<VkSemaphore> imageAvailable;
	std::vector<VkSemaphore> renderFinished;
	std::vector<VkFence> drawFences;

	// Vulkan functions
	// - create functions
	void createInstance();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSynchronization();

	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();

	void updateUniformBuffer(uint32_t imageIndex);

	// - record functions
	void recordCommands();

	// - get functions
	void getPhysicalDevice();

	// - support functions
	// -- checker functions
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool checkDeviceSuitable(VkPhysicalDevice device);

	// -- getter functions
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	SwapChainDetails getSwapChainDetails(VkPhysicalDevice device);

	// -- choose functions
	VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR> presentationModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

	// -- create functions
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkShaderModule createShaderModule(const std::vector<char>& code);
};

