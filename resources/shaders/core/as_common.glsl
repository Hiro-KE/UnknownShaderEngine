// Abstract Shader Engine - Jed Fakhfekh - https://github.com/ougi-washi

layout(push_constant) uniform push_constant_buffer
{
	vec3 camera_location;
    float _padding_0;
	vec3 camera_direction;
    float _padding_1;
	vec4 mouse_data;
	float current_time;
	int object_index;
} ps;

layout(binding = 0) uniform uniform_buffer_object 
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo; 

mat4 look_at(vec3 eye, vec3 center, vec3 up) 
{
    vec3 f = normalize(center - eye);
    vec3 r = normalize(cross(up, f));
    vec3 u = cross(f, r);

    return mat4(
        vec4(r, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    ) * mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(-eye, 1.0)
    );
}