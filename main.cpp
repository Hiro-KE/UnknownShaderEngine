#include "engine_core.h"
#include "engine_utility.h"

i32 main()
{
	as::init_window(800, 450, "Abstract Shader Engine");
	as::init_gl();

	as::world out_world;
	bool is_valid_world = as::get_world_from_file("resources/objects/dragon_world.json", false, out_world);

	as::camera* active_camera = as::find_active_camera(out_world);
	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_R))
		{
			as::get_world_from_file("resources/objects/dragon_world.json", false, out_world);
			active_camera = as::find_active_camera(out_world);
		}

		if (is_valid_world)
		{
			draw(&out_world);
			if (active_camera)
			{
				UpdateCamera(&active_camera->data, CAMERA_ORBITAL);
			}
		}
	}
	return 0;
}