#pragma once

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>

#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* newWindow);
	void cleanup(); // whenever the vkCreate*() is called, there also needs a destroy function to be called in cleanup()

	~VulkanRenderer();

private:
	GLFWwindow* window;

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

	// - Utility
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;


	// Vulkan functions
	// - create functions
	void createInstance();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();

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
};

