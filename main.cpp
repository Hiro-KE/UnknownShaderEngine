#include "vulkan_engine.h"
//#include "shaderc_core.h"


int main() 
{
	/*HelloTriangleApplication app;

		try {


			app.run();

		} catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	*/

	as::window window;
	as::vk::engine engine;
	as::init_window(window);
	as::vk::init_vulkan(engine, window);
	as::vk::start_main_loop(engine, window);
	as::vk::cleanup(engine, window);

	return EXIT_SUCCESS;
}  