#include "engine_types.h"
#include "engine_memory.h"

namespace as
{
	// general
	void init_window(const u16& width, const u16& height, const char* title);
	void init_gl();
	void set_fps(const u16& fps);
	as::engine_entity_pool* init_engine_entity_pool();
	void clear_engine_entity_pool();

	// files / entities
	std::string variable_enum_to_string(const as::entity_type& in_type);
	as::entity_type variable_string_to_enum(const std::string& in_type_str);
	bool get_updated_path(const char* in_path, const bool& absolute_path, char* out_path);
	as::world* get_world_from_file(const char* path, const bool& absolute_path);
	as::model* get_model_from_file(const char* path, const bool& absolute_path);
	as::shader* get_shader_from_file(const char* path, const bool& absolute_path);
	as::texture* get_texture_from_file(const char* path, const bool& absolute_path);
	as::camera* get_camera_from_file(const char* path, const bool& absolute_path);
	as::light* get_light_from_file(const char* path, const bool& absolute_path);
	bool add_model_to_world(as::world* world, as::model* model);
	bool add_light_to_world(as::world* world, as::light* light);
	bool add_camera_to_world(as::world* world, as::camera* camera);

	// transform
	void apply_location(const Vector3& location, Matrix& transform_matrix);
	void apply_rotation(const Vector3& rotation, Matrix& transform_matrix);
	void apply_scale(const Vector3& scale, Matrix& transform_matrix);
	void apply_transform(const Transform& transform, Matrix& transform_matrix);
	Vector3 get_location(const Matrix& transform_matrix);
	Vector3 get_position(const Matrix& transform_matrix);
	Quaternion get_rotation_quat(const Matrix& transform_matrix);
	Vector3 get_rotation_vec(const Matrix& transform_matrix);
	Vector3 get_scale(const Matrix& transform_matrix);
	i8 compare_distances(const Matrix& matrix_main, const Matrix& matrix_a, const Matrix& matrix_b);

	// entity
	void set_valid(as::entity_data& entity_data);
	void set_invalid(as::entity_data& entity_data);
	bool is_valid(const as::entity_data& entity_data);
	bool is_invalid(const as::entity_data& entity_data);

	// model
	void swap_models(as::model* model1, as::model* model2);
	void quick_sort_models(as::model** models, as::camera* camera, const i32& low, const i32& high);
	bool is_translucent(const as::model* model);
	bool is_not_translucent(const as::model* model);

	// camera
	as::camera* find_active_camera(const as::world* world);

	// light
	void update_lights_uniforms(const Shader& shader, as::light** lights, const u32& lights_count);
	void update_shadow_map(as::light* light);

	// world
	bool draw(as::world* world);
	bool draw_light_maps(as::world* world);

	// render
	void clear_background();

	// entity pool / memory
	as::world* get_empty_world_from_pool();
	as::model* get_empty_model_from_pool();
	as::shader* get_empty_shader_from_pool();
	as::texture* get_empty_texture_from_pool();
	as::camera* get_empty_camera_from_pool();
	as::light* get_empty_light_from_pool();
};

#define AS_SET_VALID(_entity) as::set_valid(_entity.entity_data)
#define AS_SET_INVALID(_entity) as::set_invalid(_entity.entity_data)
#define AS_IS_VALID(_entity) as::is_valid(_entity.entity_data)
#define AS_IS_INVALID(_entity) as::is_invalid(_entity.entity_data)

#define AS_SET_VALID_PTR(_entity) as::set_valid(_entity->entity_data)
#define AS_SET_INVALID_PTR(_entity) as::set_invalid(_entity->entity_data)
#define AS_IS_VALID_PTR(_entity) as::is_valid(_entity->entity_data)
#define AS_IS_INVALID_PTR(_entity) as::is_invalid(_entity->entity_data)

#define AS_INIT_PTR(_entity, _type) *_entity = _type()
