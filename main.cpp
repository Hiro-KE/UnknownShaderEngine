#include "engine_core.h"
#include "engine_utility.h"

i32 main()
{
	as::init_window(800, 450, "Abstract Shader Engine");
	as::init_gl();
	as::set_fps(60);
	as::init_engine_entity_pool();
	

	as::world* out_world = as::get_world("resources/objects/dragon_world.json", false);
	if (!out_world)
	{
		return false;
	}

	as::camera* active_camera = as::find_active_camera(out_world);
	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_R))
		{
			// flush
			as::clear_engine_entity_pool();
			as::init_engine_entity_pool();
			// reload world
			out_world = as::get_world("resources/objects/dragon_world.json", false);
			active_camera = as::find_active_camera(out_world);
		}

		if (out_world)
		{
			as::draw(out_world);
			if (active_camera && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				UpdateCamera(&active_camera->data, CAMERA_PERSPECTIVE);
			}
		}
	}
	return 0;
}
