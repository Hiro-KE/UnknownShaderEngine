// Abstract Shader Engine - Jed Fakhfekh - https://github.com/ougi-washi

#version 450
#include "core/as_common.glsl"
#include "as_sdf.glsl"

struct as_light 
{
    vec3 position;
    vec3 color;
    float radius;
};

struct as_mat4_array 
{
    mat4 data[128];
    int size;
};

struct as_lights_array 
{
    as_light data[128];
    int size;
};

struct as_scene 
{
    as_mat4_array objects_transforms;
    as_lights_array lights;
};

layout(set = 0, binding = 0) buffer SceneBuffer 
{
    as_scene scene;
};

layout(binding = 1) uniform sampler2D tex_sampler;

// has to be this order to match as_vertex
layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec3 frag_color;
layout(location = 3) in vec2 frag_tex_coord;
layout(location = 4) in vec3 obj_position;// instances share the same obj position so they don't work, they have to be adjusted accordingly
layout(location = 5) in flat int instance_id; 

layout(location = 0) out vec4 out_color;


sdf_result sdf_scene(vec3 p)
{
    float sphere1_dist = sd_sphere(p, 0.4);
    vec3 sphere1_color = vec3(1.0, 0.0, 0.0);

    vec3 sphere2_offset = vec3(cos(ps.current_time * (instance_id + 1) * .6), sin(ps.current_time * (instance_id + 1 ) * .2) * 2., 0.);
    float sphere2_dist = sd_sphere(p + sphere2_offset, 0.4);
    vec3 sphere2_color = vec3(0.0, 0.0, 1.0);

    float blended_dist = op_smooth_union(sphere1_dist, sphere2_dist, 0.8);
    vec3 blended_color = mix(sphere1_color, sphere2_color, smoothstep(sphere2_dist, sphere1_dist, blended_dist));

    return sdf_result(p, blended_color, blended_dist);
}


void main()
{
    vec4 clip_pos = ps.object_transform * vec4(vert_pos, 1.0);
    vec3 world_pos = (clip_pos.xyz / clip_pos.w) - obj_position;
    vec3 ray_dir = normalize(world_pos - ps.camera_location);
    sdf_result sdf = raymarch(ps.camera_location, ray_dir);
    vec3 color = sdf.color;

    if(sdf.alpha < .4)
    {
        discard;
    }

    vec3 light_dir = vec3(1, -20, 10);
    float light_mask = dot(frag_normal.xyz, light_dir);

    color = vec3(clamp(smoothstep(.1, 30., light_mask), 0., 1.) + .003) * color;
    out_color = vec4(color, 1.);
}
