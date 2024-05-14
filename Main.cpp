#define GLFW_INCLUDE_VULKAN /* Library used to create a window to allow for real time rendering*/
#include <GLFW/glfw3.h>

#include <iostream> 
#include <stdexcept> /* These two headers report and propogate errors*/
#include <vector> /* Include vector type data */
#include <cstring> /* Needed for strcamp function */
#include <cstdlib> /* This header provides macros for exit_success and _failure*/
#include <optional> /* header to allow us to query whether a value exists or not */

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600; /* Width and Height of Window*/

///////////////////////////* Code for vailadation layers (debugging) */////////////////////////////////////

/* Configuration variables to specify validation layers */
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

/* proxy function to handle create debug utils structure */
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

/* proxy function to destroy debug utils messenger object */
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessnger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessnger, pAllocator);

	}
}

/* structure to store queue family values*/
	/* Data value is optional as queue family may not exist*/
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	bool isComplete() {
	return graphicsFamily.has_value();
	}
};
//////////////////////////////*END_OFF*/////////////////////////////////////////////////////////////////////////

/* The program is wrapped into a class that will store the vulkan objects as private class members
and functions to initiate them. 

These will be called in init_vulcan

initWindow function is used to initalise GLFW and create a window

mainLoop iterates until the window is closed

cleanup will deallocate the memory resources 

Every resultant feature will be a new function to be called from initVulkan*/


class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}
private:
	
	GLFWwindow* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	void initWindow() {

		glfwInit(); /* initialises GLFW library*/

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); /* telling GLFW not to create an OpenGL context*/
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); /* handling resized windows*/

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Triangle Demo", nullptr, nullptr); /* initialising window*/
	}
	
	void initVulkan() {
		createInstance(); /* Invoking vulkan instance*/
		setupDebugMessenger(); /* Setting up debug messenger*/
		pickPhysicalDevice(); /* Stores graphics card in physical device handle*/
	}

	void setupDebugMessenger() {
		/*exit function if not debugging*/
		if (!enableValidationLayers) return;

		/* structure with details about messenger and callbacks*/
		/* structure lets us filter which message severities and types we get notified about*/

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up a debug messenger!");
		}
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}
	void createInstance() { 
		/* checking validation layers */
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available");
		}
		
		/* Application specific information */
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		/* Structure that tells vulkan driver which global extensions and validation layers to use*/
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		/* Vk instance debug information*/
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			
			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}

	};
	/* Function to check if all requested layers are available */
	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		/* Checking if all layers in validation layers exist in available layers*/
		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	/* function to return the required list of extensions based on whether validation layers are enabled or not*/
	
	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	/* Debug callback fucntion with parameters, message severity, message type */

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	/* function creating debug messenger instance and setting a pointer to pNext */

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	void pickPhysicalDevice() {

		/* Similar process to listing extensions*/
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		/* Stopping if no graphics card exists*/

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan Support");
		}

		/* Creating array to hold all phyical device handles */

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		/* Checking capabilities of graphics card */

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU");
		}
	}

	/* bool that stores whether a graphics card is suitable, calls the queue family finder function*/

	bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);

		return indices.isComplete();

	}

	/* checking if a graphics card can support the queue families we need for our application*/

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {

			/* break loop if queue is found that supports graphics operations*/

			if (indices.isComplete()) {
				break;
			}
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			i++;
		}
		return indices;
	};
	
};
	

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}