#define GLFW_INCLUDE_VULKAN /* Library used to create a window to allow for real time rendering*/
#include <GLFW/glfw3.h>

#include <iostream> 
#include <stdexcept> /* These two headers report and propogate errors*/
#include <cstdlib> /* This header provides macros for exit_success and _failure*/

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600; /* Width and Height of Window*/

uint32_t glfwExtensionCount = 0;
const char** glfwExtensions; /* Definitions for the global extensions to interface with windows*/

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
		createInstance();
		initVulkan();
		mainLoop();
		cleanup();

	}
private:
	void initWindow() {

		glfwInit(); /* initialises GLFW library*/

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); /* telling GLFW not to create an OpenGL context*/
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); /* handling resized windows*/

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Triangle Demo", nullptr, nullptr); /* initialising window*/
	}
	GLFWwindow* window; /* stores a refernce to window*/

	void createInstance() { /* Application specific information */
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
	}

	void initVulkan() {
		createInstance(); /* Invoking vulkan instance*/

		/* Structure that tells vulkan driver which global extensions and validation layers to use*/
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		/* glfw function that returns extensions needed for the window system*/
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;

		/*Everything is specified now we can create an vlcreateinstance call*/
		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

		/*Check if an instance is created successfully*/
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance");
		}
	}
	VkInstance instance; /* Data member to hold the instance */

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

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