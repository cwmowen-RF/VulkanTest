#define GLFW_INCLUDE_VULKAN /* Library used to create a window to allow for real time rendering*/
#include <GLFW/glfw3.h>

#include <iostream> 
#include <stdexcept> /* These two headers report and propogate errors*/
#include <cstdlib> /* This header provides macros for exit_success and _failure*/

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600; /* Width and Height of Window*/

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
	void initWindow() {

		glfwInit(); /* initialises GLFW library*/

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); /* telling GLFW not to create an OpenGL context*/
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); /* handling resized windows*/

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Triangle Demo", nullptr, nullptr); /* initialising window*/
	}
	GLFWwindow* window; /* stores a refernce to window*/

	void initVulkan() {

	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
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