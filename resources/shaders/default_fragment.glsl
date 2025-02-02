// Abstract Shader Engine - Jed Fakhfekh - https://github.com/ougi-washi

#version 450

#include "core/as_common.glsl"
#include "core/as_fragment_layout.glsl"
#include "sdf/as_sdf.glsl"

sdf_result sdf_scene(vec3 p)
{
    // vec3 light_dir = vec3(1, -20, 10);
    // float light_mask = dot(frag_normal.xyz, light_dir);
    // vec3 light_color = vec3(clamp(smoothstep(.1, 30., light_mask), 0., 1.) + .003);

    float blended_dist = SDF_MAX_DIST;
    vec3 blended_color = vec3(0.);
    for (int i = 0 ; i < get_object_count() ; i++)
    {
        float sphere_dist = SDF_MAX_DIST;
        vec3 sphere_color = vec3(0.);
        if (i == 0)
        {
            sphere_dist = sd_sphere(p - get_object_position(i), .6);
            sphere_color =  vec3(1.0, 0., 0.0);
        }
        else if (i == 1)
        {
            sphere_dist = sd_sphere(p - get_object_position(i), 0.6);
            sphere_color =   vec3(0., 1., 0.);
        }   
        else if (i == 2)
        {
            sphere_dist = sd_sphere(p - get_object_position(i), 0.3);
            sphere_color =   vec3(0., .0, 1.);
        }
        float color_weight = 1. - smoothstep(0.0, 1., sphere_dist);
        blended_dist = op_smooth_union(blended_dist, sphere_dist, 1.);
        blended_color = mix(blended_color, sphere_color, color_weight);
    }
    
    return sdf_result(p, blended_color, blended_dist);
}

void main()
{
    const vec3 world_vert_pos = vert_pos + get_current_object_position() ;
    const vec3 world_cam_pos = get_camera_pos() - get_current_object_position();
    const vec3 ray_dir = normalize(world_vert_pos - world_cam_pos);
    const sdf_result sdf = raymarch(world_cam_pos, ray_dir);
    if(sdf.alpha < .4) { discard; }
    out_color = vec4(sdf.color, 1.);
}
