#pragma once

#include "types.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <chrono>

#define MAX_BONE_INFLUENCE 4

namespace as
{
	enum variable_type
	{
		FLOAT = 0x1406,
		INT = 0x1404,
		BOOL = 0x8B56,
		TEXTURE = 0x1702,
		VECTOR // ??
	};

	struct texture
	{
		u32 id;
		char uniform_name[256] = "";
		char path[256] = "";
		i32 width;
		i32 height;
		i32 number_of_channels;
	};

	struct uniform
	{
		u32 id;
		i32 size;
		variable_type type;
		u8 normalized : 1;
		i32 stride;
		u64 position = 0;
	};

	struct shader
	{
		// external
		i32 vertex_shader;
		i32 fragment_shader;
		std::vector<as::uniform> uniforms; // to change from vector
		std::vector<as::texture> textures; // to change from vector (this can be changed to an array of pointers later if needed.

		// internal
		u32 shader_program = 0;
	};

	struct vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex_coords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		i32 bone_ids[MAX_BONE_INFLUENCE];
		float weights[MAX_BONE_INFLUENCE];
	};

	struct mesh
	{
		// model
		std::vector<vertex> vertices;
		std::vector<u32> indices;

		// shader
		as::shader* shader_ptr = nullptr;

		// rendering
		u32 VBO = 0; // Vertex Buffer Object
		u32 EBO = 0; // Element Buffer Object
		u32 VAO = 0; // Vertex array Object (currently we're using 1 VAO per mesh, we should maybe combine the buffers for bulk rendering)
	};

	struct model
	{
		char path[500] = "";
		std::vector<as::mesh> meshes;
		glm::mat4 transform = glm::mat4(1.f);
	};
	
	struct camera
	{
		// camera Attributes
		glm::vec3 position = glm::vec3(0.f);
		glm::vec3 front = glm::vec3(0.f, 0.f, -1.f);
		glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 right;
		glm::vec3 world_up;

		// euler Angles
		glm::vec3 rotation; // X : roll // Y : pitch // Z : yaw

		// camera options
		f32 movement_speed = 1.f;
		f32 mouse_sensitivity = 1.f;
		f32 zoom;
		f32 zoom_speed = 1.f;
		f32 fov = glm::radians(45.f);
		f32 near_plane = .1f;
		f32 far_plane = 100.f;
		f32 aspect_ratio = 1.3f;
	};

	namespace timer
	{
		struct handle
		{
			std::chrono::system_clock::time_point start;
		};
	};
};