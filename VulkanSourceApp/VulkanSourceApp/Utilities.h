#pragma once

#include <fstream>

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

static std::vector<char> readFile(const std::string& filename)
{
	// open stream from given file
	// std::ios::binary tells stream to read file as binra
	// std::ios::ate tells stream to start reading from end of file
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	// check if file stream succesfully opened
	if (!file.is_open())
	{
		throw std::runtime_error("faild to open a file");
	}

	// get current read position and use to resize the file buffer
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> fileBuffer(fileSize);

	// move read position (seek to) the start of the file
	file.seekg(0);

	//read the file data into the buffer(stream "fileSize" in total)
	file.read(fileBuffer.data(), fileSize);

	//close stream
	file.close();

	return fileBuffer;
}