#include "engine_types.h"

namespace as
{
	// generic 
	void delete_vertex_array(const u32& VAO);

	// shaders
	bool create_shader(const char* vert_shader_src, const char* frag_shader_src, as::shader& out_shader);
	void create_shader_program(u32& out_shader_program);
	bool bind_shaders_to_program(const u32& shader_program, const as::shader& shader_to_attach);
	void delete_shader(const as::shader& shader);
	void delete_shader_program(const u32& shader_program);

	// objects
	bool initialize_object(const float* vertices, const u32& vertices_count, const float* indices, const u32& indices_count, u32& VAO, as::object& out_object);
	void assign_shader(as::object& object, as::shader& shader);
	void delete_object_data(as::object* object);

	// render
	void clear_background();
	void draw(const u32& shader_program, const u32& VAO);
};