#pragma once

#include <fstream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const int MAX_FRAME_DRAWS = 2;
const int MAX_OBJECTS = 100;

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct Vertex
{
	glm::vec3 pos;		// vertex position (x, y, z)
	glm::vec3 col;		// vertex color (r, g, b)
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

static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties)
{
	// get properties of physical device memory
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		// bit shift: e.g. 0001 << 1 == 0010, 0001 << 2 == 0100
		if ((allowedTypes & (1 << i))																									// index of memory type must match corresponding bit in allowedTypes
			&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)		// desired property bit flags are part of memory type's property flags
		{
			// this memory type is valid, so return its index
			return i;
		}
	}
}

static void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	// CREATE VERTEX BUFFER
	// information to create a buffer (doesn't include assigning memory)
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;																// size of buffer (size of 1 vertex * number of vertices)
	bufferInfo.usage = bufferUsage;														// multiples of buffer possible
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;			// similar to swapchain images, can share vertex buffers

	VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Faild to create a vertex buffer");
	}

	// GET BUFFER MEMORY REQUIREMENT
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, *buffer, &memoryRequirements);

	// ALLOCATE MEMORY TO BUFFER
	VkMemoryAllocateInfo memoryAllocInfo = {};
	memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocInfo.allocationSize = memoryRequirements.size;
	memoryAllocInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memoryRequirements.memoryTypeBits,	 bufferProperties);																																									
	// memoryTypeBits £ºindex of memory type on physical device that has required bit flags
	// bufferPorperties	:
	// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT		:	CPU can interact with memory
	// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	:	allows placement of data straight into buffer after mapping (otherwise would have to specify manually)


	// allocate memory to VkDeviceMemory
	result = vkAllocateMemory(device, &memoryAllocInfo, nullptr, bufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Faild to allocate vertex buffer memory");
	}

	// allocate memory to given vertex buffer
	vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}

static VkCommandBuffer beginCommandBuffer(VkDevice device, VkCommandPool commandPool)
{
	// command buffer to hold transfer commands
	VkCommandBuffer commandBuffer;

	// command buffer details
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	// allocate command buffer from pool
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	// information to begin the command buffer record
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;	// we are only using the command buffer once, so set up for one time submit

	// begin recording transfer commands
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

static void endAndSubmitCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer)
{
	// end commands
	vkEndCommandBuffer(commandBuffer);

	// queue submission information
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	// submit transfer command to transfer queue and wait until it finishes
	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	// free temporary command buffer back to pool
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

static void copyBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool,
	VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
{
	// create buffer
	VkCommandBuffer transferCommandBuffer = beginCommandBuffer(device, transferCommandPool);

	// region of data to copy form and to
	VkBufferCopy bufferCopyRegion = {};
	bufferCopyRegion.srcOffset = 0;
	bufferCopyRegion.dstOffset = 0;
	bufferCopyRegion.size = bufferSize;

	// command to copy src buffer to dst buffer
	vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyRegion);

	endAndSubmitCommandBuffer(device, transferCommandPool, transferQueue, transferCommandBuffer);
}

static void copyImageBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkImage image, uint32_t width, uint32_t height)
{
	// create buffer
	VkCommandBuffer transferCommandBuffer = beginCommandBuffer(device, transferCommandPool);

	VkBufferImageCopy imageRegion = {};
	imageRegion.bufferOffset = 0;																					// offset into data
	imageRegion.bufferRowLength = 0;																			// row length of data to calculate data spacing
	imageRegion.bufferImageHeight = 0;																			// image height to calculate data spacing
	imageRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;		// which aspect of image to copy
	imageRegion.imageSubresource.mipLevel = 0;															// Mipmap level to copy
	imageRegion.imageSubresource.baseArrayLayer = 0;													// starting array layer (if array)
	imageRegion.imageSubresource.layerCount = 1;														// number of layers to copy starting at baseArrayLayer
	imageRegion.imageOffset = { 0, 0, 0 };																		// offset into image (as opposet to raw data in bufferOffset)
	imageRegion.imageExtent = { width, height, 1 };															// size of region to copy as (x, y, z) values

	// copy buffer to given image
	vkCmdCopyBufferToImage(transferCommandBuffer, srcBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageRegion);

	endAndSubmitCommandBuffer(device, transferCommandPool, transferQueue, transferCommandBuffer);
}

static void transitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	// create buffer
	VkCommandBuffer commandBuffer = beginCommandBuffer(device,commandPool);

	VkImageMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.oldLayout = oldLayout;																		// layout to transition from
	memoryBarrier.newLayout = newLayout;																	// layout to transition to
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;						// queue family to transition from
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;						// queue family to transition to
	memoryBarrier.image = image;																					// image being accessed and modified as part of barrier
	memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;	// aspect of image being altered
	memoryBarrier.subresourceRange.baseMipLevel = 0;													// first mip level to start alterations on
	memoryBarrier.subresourceRange.levelCount = 1;														// number of mip levels to alter starting from baseMipLevel (1 is the inital image)
	memoryBarrier.subresourceRange.baseArrayLayer = 0;												// first layer to start alterations on
	memoryBarrier.subresourceRange.layerCount = 1;														// number of layers to alter starting from baseArrayLayer

	VkPipelineStageFlags srcStage;
	VkPipelineStageFlags dstStage;

	// if transitioning from new image to image ready to receive data...
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		memoryBarrier.srcAccessMask = 0;																// memory access stage transition must after...
		memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;			// memory access stage transition must before...
		
		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} // if transitioning from transfer destination to shader readable...
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;						// textures are usually read from in the fragment shader, so make sure it is ready before shader reads it
	}
	
	vkCmdPipelineBarrier(
		commandBuffer,
		srcStage, dstStage,								// Pipeline stages (match to src and dst AccessMasks)
		0,									// dependency flags
		0, nullptr,						// memory barrier count + data
		0, nullptr,						// buffer memory barrier count + data
		1, &memoryBarrier		// image memory barrier count + data
	);

	endAndSubmitCommandBuffer(device, commandPool, queue, commandBuffer);
}