// internal dependencies (engine)
#include "engine_utility.h"
#include "engine_core.h"
#include "rlgl.h"

void as::init_gl()
{
	rlEnableDepthTest();
	rlEnableColorBlend();
	rlSetBlendMode(RL_BLEND_ALPHA);
	rlEnableBackfaceCulling();
}

void as::init_window(const u16& width, const u16& height, const char* title)
{
	SetConfigFlags(FLAG_VSYNC_HINT);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(width, height, title);
}

std::string get_path(const json& json_data, const char* path_json_type)
{
	if (json_data.contains(path_json_type))
	{
		std::string path = json_data[path_json_type].get<std::string>();
		if (json_data.contains("absolute_path"))
		{
			if (json_data["absolute_path"].get<bool>())
			{
				return path;
			}
			else
			{
				return as::util::get_current_path() + "/../" + path;
			}
		}
	}
	return std::string();
}

bool get_bool(const json& json_data, const char* json_field_name, bool& out_bool)
{
	if (json_data.contains(json_field_name))
	{
		out_bool = json_data[json_field_name].get<bool>();
		return true;
	}
	return false;
}

bool get_float(const json& json_data, const char* json_field_name, f32& out_float)
{
	if (json_data.contains(json_field_name))
	{
		out_float = json_data[json_field_name].get<f32>();
		return true;
	}
	return false;
}

bool get_vec3(const json& json_data, const char* json_field_name, Vector3& out_vector)
{
	if (json_data.contains(json_field_name) && json_data[json_field_name].size() >= 3)
	{
		out_vector.x = json_data[json_field_name][0];
		out_vector.y = json_data[json_field_name][1];
		out_vector.z = json_data[json_field_name][2];
		return true;
	}
	return false;
}

bool get_vec2(const json& json_data, const char* json_field_name, Vector2& out_vector)
{
	if (json_data.contains(json_field_name) && json_data[json_field_name].size() >= 2)
	{
		out_vector.x = json_data[json_field_name][0];
		out_vector.y = json_data[json_field_name][1];
		return true;
	}
	return false;
}

bool get_transform(const json& json_data, Transform& out_transform)
{
	if (json_data.contains("transform"))
	{
		json transform_json_data = json_data["transform"];
		bool is_valid_location = get_vec3(transform_json_data, "location", out_transform.translation);
		Vector3 rotation = Vector3();
		bool is_valid_rotation = get_vec3(transform_json_data, "rotation", rotation);
		if (is_valid_rotation)
		{
			out_transform.rotation = QuaternionFromEuler(rotation.x, rotation.y, rotation.z);
		}
		bool is_valid_scale = get_vec3(transform_json_data, "scale", out_transform.scale);
		return is_valid_location && is_valid_rotation && is_valid_scale;
	}
	return false;
}

std::string as::variable_enum_to_string(const as::entity_type& in_type)
{
	return entity_type_strings[in_type];
}

as::entity_type as::variable_string_to_enum(const std::string& in_type_str)
{
	for (u8 i = 0; i < entity_type_strings.size(); i++)
	{
		if (entity_type_strings[i] == in_type_str)
		{
			return (entity_type)i;
		}
	}
	return as::entity_type::NONE;
}

as::entity_type get_type(const json& json_data)
{
	if (json_data.contains("type"))
	{
		std::string entity_type = json_data["type"].get<std::string>();
		return as::variable_string_to_enum(entity_type);
	}
	return as::NONE;
}

void as::set_path(const std::string& path, const bool& absolute_path, entity_data& out_entity_data)
{
	std::string new_path;
	if (absolute_path)
	{
		new_path = path;
	}
	else
	{
		new_path = as::util::get_current_path() + "/../" + path;
	}
	memcpy(out_entity_data.path, new_path.c_str(), 256);
}

as::world* as::get_world_from_file(const std::string& path, const bool& absolute_path)
{
	AS_LOG(LV_LOG, "Parse file [" + path + "]");

	set_path(path, absolute_path, out_world.entity_data);
	json json_data = as::util::read_json_file(out_world.entity_data.path);

	if (get_type(json_data) == as::entity_type::WORLD)
	{
		as::world out_world = get_incremental_world_from_pool();

		if (json_data.contains("models"))
		{
			std::vector<std::string> models_file_paths = json_data["models"].get<std::vector<std::string>>();
			out_world.models_count = models_file_paths.size();
			for (u16 i = 0; i < out_world.models_count; i++)
			{
				out_world.models[i];
				get_model_from_file(models_file_paths[i], absolute_path, engine_memory_pool.models[engine_memory_pool.models_count++]);
			}
		}
		if (json_data.contains("is_active"))
		{
			bool is_active = false;
			if (get_bool(json_data, "is_active", is_active))
			{
				out_world.is_active = is_active;
			}
		}
		return &out_world;
	}
	return nullptr;
}

bool as::get_model_from_file(const std::string& path, const bool& absolute_path, as::model& out_model)
{
	AS_LOG(LV_LOG, "Parse file [" + path + "]");
	set_path(path, absolute_path, out_model.entity_data);
	json json_data = as::util::read_json_file(out_model.entity_data.path);

	if (get_type(json_data) == as::entity_type::MODEL)
	{
		if (json_data.contains("path"))
		{
			std::string model_path = as::util::get_current_path() + "/../" + json_data["path"].get<std::string>();
			out_model.data = LoadModel(model_path.c_str());
		}
		if (json_data.contains("shaders"))
		{
			std::vector<std::string> shaders = json_data["shaders"].get<std::vector<std::string>>();
			for (u16 i = 0; i < shaders.size(); i++)
			{
				as::shader found_shader;
				if (get_shader_from_file(shaders[i], absolute_path, found_shader) && out_model.data.materialCount > i)
				{
					out_model.data.materials[i].shader = found_shader.data;
				}
			}
			if (out_model.data.materialCount > shaders.size())
			{
				as::shader found_shader;
				if (get_shader_from_file(DEFAULT_SHADER_PATH, absolute_path, found_shader))
				{
					for (u16 i = 0; i < out_model.data.materialCount; i++)
					{
						out_model.data.materials[i].shader = found_shader.data;
					}
				}
			}
		}
		else
		{
			AS_LOG(LV_WARNING, "Model json file does not have shaders");
			return false;
		}
		out_model.data.transform = MatrixIdentity();
		Vector3 out_vector = Vector3();
		if (get_vec3(json_data, "scale", out_vector))
		{
			as::apply_scale(out_vector, out_model.data.transform);
		};
		if (get_vec3(json_data, "rotation", out_vector))
		{
			as::apply_rotation(out_vector, out_model.data.transform);
		};
		if (get_vec3(json_data, "location", out_vector))
		{
			as::apply_location(out_vector, out_model.data.transform);
		};
		return true;
	}
	return false;
}


bool as::get_shader_from_file(const std::string& path, const bool& absolute_path, as::shader& out_shader)
{
	AS_LOG(LV_LOG, "Parse file [" + path + "]");
	set_path(path, absolute_path, out_shader.entity_data);
	json json_data = as::util::read_json_file(out_shader.entity_data.path);

	if (get_type(json_data) == as::entity_type::SHADER)
	{
		std::string vertex_path;
		std::string fragment_path;
		if (json_data.contains("vertex_path"))
		{
			vertex_path = as::util::get_current_path() + "/../" + json_data["vertex_path"].get<std::string>();
		}
		else
		{
			AS_LOG(LV_WARNING, "Shader json file does not have vertex shader");
			return false;
		}
		if (json_data.contains("fragment_path"))
		{
			fragment_path = as::util::get_current_path() + "/../" + json_data["fragment_path"].get<std::string>();
		}
		else
		{
			AS_LOG(LV_WARNING, "Shader json file does not have fragment shader");
			return false;
		}
		out_shader.data = LoadShader(vertex_path.c_str(), fragment_path.c_str());
		if (out_shader.data.id > 0)
		{
			return true;
		}
		else
		{
			AS_LOG(LV_WARNING, "Could not create the shader");
		}
	}
	return false;


}


bool as::get_texture_from_file(const std::string& path, const bool& absolute_path, as::texture& out_texture)
{
	AS_LOG(LV_LOG, "Parse file [" + path + "]");
	set_path(path, absolute_path, out_texture.entity_data);
	json json_data = as::util::read_json_file(out_texture.entity_data.path);

	if (get_type(json_data) == as::entity_type::TEXTURE)
	{
		if (json_data.contains("path"))
		{
			std::string texture_path = as::util::get_current_path() + "/../" + json_data["path"].get<std::string>();

			bool found_texture = false;
			// TODO: caching
			//for (size_t i = 0; i < allocations_count; i++)
			//{
			//	as::entity* found_entity_ptr = static_cast<as::entity*>(allocations[i]->ptr);
			//	if (found_entity_ptr && found_entity_ptr->type == as::TEXTURE)
			//	{
			//		as::texture* found_texture_ptr = static_cast<as::texture*>(found_entity_ptr->data_ptr);
			//		std::string path_string = util::get_current_path() + "/../" + std::string(texture_path);
			//		if (found_texture_ptr && strcmp(found_texture_ptr->path, path_string.c_str()) == 0)
			//		{
			//			AS_LOG(LV_LOG, "Texture " + texture_path + "already defined, using the found pointer.");
			//			out_entity = found_entity_ptr;
			//			found_texture = true;
			//		}
			//	}
			//}
			if (!found_texture)
			{
				out_texture.data = LoadTexture(texture_path.c_str());
				if (out_texture.data.id <= 0)
				{
					return true;
				}
				else
				{
					AS_LOG(LV_WARNING, "Could not load the texture");
					return false;
				}
			}
		}
		else
		{
			AS_LOG(LV_WARNING, "Texture json file does not have path");
			return false;
		}
	}
}

bool as::get_camera_from_file(const std::string& path, const bool& absolute_path, as::camera& out_camera)
{
	AS_LOG(LV_LOG, "Parse file [" + path + "]");
	set_path(path, absolute_path, out_camera.entity_data);
	json json_data = as::util::read_json_file(out_camera.entity_data.path);

	if (get_type(json_data) == as::entity_type::CAMERA)
	{
		get_vec3(json_data, "position", out_camera.data.position);
		get_vec3(json_data, "target", out_camera.data.target);
		if (json_data.contains("is_active"))
		{
			out_camera.is_active = json_data["is_active"].get<bool>();
		}
		Camera camera = { 0 };
		out_camera.data.up = Vector3(0.0f, 1.0f, 0.0f);          // Camera up vector (rotation towards target)
		out_camera.data.fovy = 75.f;                                // Camera field-of-view Y
		out_camera.data.projection = CAMERA_PERSPECTIVE;                   // Camera mode type
		return true;
	}
	return false;
}

bool as::get_light_from_file(const std::string& path, const bool& absolute_path, as::light& out_light)
{
	AS_LOG(LV_LOG, "Parse file [" + path + "]");
	set_path(path, absolute_path, out_light.entity_data);
	json json_data = as::util::read_json_file(out_light.entity_data.path);

	if (get_type(json_data) == as::entity_type::LIGHT)
	{
		get_vec3(json_data, "location", out_light.location);
		get_float(json_data, "intensity", out_light.intensity);
		get_float(json_data, "attenuation", out_light.intensity);
		get_vec3(json_data, "color", out_light.color);
		return true;
	}
	return false;
}

void as::apply_location(const Vector3& location, Matrix& transform_matrix)
{
	Matrix translation = MatrixTranslate(location.x, location.y, location.z);
	transform_matrix = MatrixMultiply(transform_matrix, translation);
}

void as::apply_rotation(const Vector3& rotation, Matrix& transform_matrix)
{
	Matrix rotationX = MatrixRotateX(DEG2RAD * rotation.x);
	Matrix rotationY = MatrixRotateY(DEG2RAD * rotation.y);
	Matrix rotationZ = MatrixRotateZ(DEG2RAD * rotation.z);
	transform_matrix = MatrixMultiply(transform_matrix, rotationX);
	transform_matrix = MatrixMultiply(transform_matrix, rotationY);
	transform_matrix = MatrixMultiply(transform_matrix, rotationZ);
}

void as::apply_scale(const Vector3& scale, Matrix& transform_matrix)
{
	Matrix scale_matrix = MatrixScale(scale.x, scale.y, scale.z);
	transform_matrix = MatrixMultiply(transform_matrix, scale_matrix);
}

void as::apply_transform(const Transform& transform, Matrix& transform_matrix)
{
	apply_location(transform.translation, transform_matrix);
	apply_rotation(QuaternionToEuler(transform.rotation), transform_matrix);
	apply_scale(transform.scale, transform_matrix);
}


bool as::is_valid(const as::entity_data& entity_data)
{
	return entity_data.is_valid;
}

as::camera* as::find_active_camera(const as::world& world)
{
	for (u32 i = 0 ; i < world.cameras_count ; i++)
	{
		if (world.cameras[i] && world.cameras[i]->is_active)
		{
			return world.cameras[i];
		}
	}
	return nullptr;
}

void as::update_lights_uniforms(const Shader& shader, light** lights, const u32& lights_count)
{
	if (lights_count > 0)
	{
		{
			const i32 light_count_loc = GetShaderLocation(shader, "lights_count");
			SetShaderValue(shader, light_count_loc, &lights_count, SHADER_UNIFORM_INT);
		}

		for (u32 i = 0; i < lights_count; i++)
		{
			if (lights[i])
			{
				char buffer[64];
				i32 light_data_loc = -1;

				sprintf(buffer, "lights[%i].location", i);
				light_data_loc = GetShaderLocation(shader, buffer);
				SetShaderValue(shader, light_data_loc, &lights[i]->location, SHADER_UNIFORM_VEC3);

				sprintf(buffer, "lights[%i].color", i);
				light_data_loc = GetShaderLocation(shader, buffer);
				SetShaderValue(shader, light_data_loc, &lights[i]->color, SHADER_UNIFORM_VEC3);

				sprintf(buffer, "lights[%i].intensity", i);
				light_data_loc = GetShaderLocation(shader, buffer);
				SetShaderValue(shader, light_data_loc, &lights[i]->intensity, SHADER_UNIFORM_FLOAT);

				sprintf(buffer, "lights[%i].attenuation", i);
				light_data_loc = GetShaderLocation(shader, buffer);
				SetShaderValue(shader, light_data_loc, &lights[i]->attenuation, SHADER_UNIFORM_FLOAT);
			}
		}
	}
}

bool as::draw(const as::world* world)
{
	if (world)
	{
		std::vector<const as::model*> models_to_draw;
		as::camera* camera_to_use = nullptr;
		for (u16 i = 0; i < world->entities_count; i++)
		{	
			if (world->entities[i])
			{
				as::model* out_model = nullptr;
				as::camera* out_camera = nullptr;
				if (get_model_from_entity(world->entities[i], out_model))
				{
					models_to_draw.push_back(out_model);
				}
				else if (get_camera_from_entity(*world->entities[i], out_camera))
				{
					if (out_camera && out_camera->is_active)
					{
						camera_to_use = out_camera;
					}
				}
			}
		}

		AS_ASSERT(!models_to_draw.empty(), "Cannot draw 0 models, check the world content");
		AS_ASSERT(camera_to_use, "Cannot draw with no active camera, check the world content");

		// get all lights
		as::light** lights = nullptr;
		const u32 lights_count = get_all_lights(world, lights);

		BeginDrawing();
		clear_background();
		BeginMode3D(camera_to_use->data);
		for (const as::model* model : models_to_draw)
		{
			if (model)
			{
				for (u32 j = 0; j < model->data.materialCount; j++)
				{
					update_lights_uniforms(model->data.materials[j].shader, lights, lights_count);
				}
				Vector3 position = Vector3(model->data.transform.m12, model->data.transform.m13, model->data.transform.m14);
				DrawModel(model->data, position, 1.0, WHITE);
			}
		}
		DrawGrid(30, 10.f);
		EndMode3D();
		EndDrawing();

		AS_FREE(lights);
	}
	return false;
}

void as::clear_background()
{
	ClearBackground(Color(0.f, 0.f, 0.f));
}

as::world& as::get_incremental_world_from_pool()
{
	engine_memory_pool.worlds_count++;
	return engine_memory_pool.worlds[engine_memory_pool.worlds_count - 1];
}

as::model& as::get_incremental_model_from_pool()
{
	engine_memory_pool.models_count++;
	return engine_memory_pool.models[engine_memory_pool.models_count - 1];
}

as::shader& as::get_incremental_shader_from_pool()
{
	engine_memory_pool.shaders_count++;
	return engine_memory_pool.shaders[engine_memory_pool.shaders_count - 1];
}

as::texture& as::get_incremental_texture_from_pool()
{
	engine_memory_pool.textures_count++;
	return engine_memory_pool.textures[engine_memory_pool.textures_count - 1];
}

as::light& as::get_incremental_light_from_pool()
{
	engine_memory_pool.lights_count++;
	return engine_memory_pool.lights[engine_memory_pool.lights_count - 1];
}

as::camera& as::get_incremental_camera_from_pool()
{
	engine_memory_pool.cameras_count++;
	return engine_memory_pool.cameras[engine_memory_pool.cameras_count - 1];
}
