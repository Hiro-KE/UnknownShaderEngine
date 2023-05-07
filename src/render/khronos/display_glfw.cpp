#include "types.h"
#include "khronos/display_glfw.h"
#include "glad/gl.h"
#include <GLFW/glfw3.h>

as::input_handle* input;

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (input)
	{
		as::key_params params;
		params.input_event = static_cast<as::input_event_type>(action);
		params.type = static_cast<as::key_type>(key);
		input->on_receive_input(params);
	}
}

bool as::create_window(GLFWwindow*& window, const u16& width, const u16& height, as::input_handle& in_input)
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// glfw window creation https://github.com/matusnovak/rpi-opengl-without-x
	// --------------------
	window = glfwCreateWindow(width, height, "Abstract Shader Engine", NULL, NULL);
	if (window == NULL)
	{
		AS_LOG(LV_ERROR, "Failed to create GLFW window");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	// input
	input = &in_input;
	glfwSetKeyCallback(window, key_callback);
	
	// glad: load all OpenGL function pointers with glfw
	// ---------------------------------------
	if (gladLoadGL(glfwGetProcAddress))
	{
		return true;
	}
	else
	{
		AS_LOG(LV_ERROR, "Failed to initialize OpenGL context with GLFW");
	}
	return false;
}

bool as::should_loop(GLFWwindow* window)
{
	if (window)
	{
		return !glfwWindowShouldClose(window);
	}
	return false;
}

bool as::process_input(GLFWwindow* window, as::input_handle& input_handle)
{
	if (window)
	{
		//as::key_params params;
		////params.input_event = 
		////input_handle.on_input(,)
		//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		//{
		//	glfwSetWindowShouldClose(window, true);
		//}
		return true;
	}
	return false;
}

bool as::update(GLFWwindow* window)
{
	if (window)
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
		return true;
	}
	return false;
}

bool as::get_window_size(GLFWwindow* window, i32& width, i32& height)
{
	if (window)
	{
		glfwGetWindowSize(window, &width, &height);
		return true;
	}
	return false;
}

f32 as::get_display_ratio(GLFWwindow* window)
{
	if (window)
	{
		i32 width, height;
		if (get_window_size(window, width, height))
		{
			return (f32)width/ (f32)height;
		}
	}
	return 1.33f;
}

void as::terminate_glfw()
{
	glfwTerminate();
}