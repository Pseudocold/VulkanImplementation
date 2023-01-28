#pragma once

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Indices (locations) of Queue Families (if they exist at all)
struct QueueFamilyIndices {
	int graphicsFamily = -1;	// location of Graphics Queue Family
	int presentationFamily = -1; // location of Presentation Queue Family

	//check if queue families are valid
	bool isValid()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};


struct SwapChainDetails {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;			// suface properties, e.g. image size/extent
	std::vector<VkSurfaceFormatKHR> formats;				// surface image formats e.g. RGBA and size of each color
	std::vector<VkPresentModeKHR> presentationModes;		// how images should be presented to screen
};

struct SwapchainImage {
	VkImage image;
	VkImageView imageView;
};