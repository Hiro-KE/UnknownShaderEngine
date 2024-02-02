// Abstract Shader Engine - Jed Fakhfekh - https://github.com/ougi-washi

#include "core/as_render_queue.h"
#include "as_memory.h"

void* as_render_queue_thread_run(as_render_queue* queue)
{
	if (AS_IS_INVALID(queue)) { return NULL; }
	while (queue->is_running)
	{
		const sz queue_size = queue->commands.size;
		if (queue_size > 0)
		{
			AS_WAIT_AND_LOCK(queue);
			as_render_command* command = &queue->commands.data[queue_size - 1];
			if (command)
			{
				command->func_ptr(command->arg);
			}
			AS_ARRAY_REMOVE_AT(queue->commands, queue_size - 1);
			AS_SET_UNLOCKED(queue);
		}
		else
		{
			sleep_seconds(AS_RENDER_QUEUE_REST_TIME);
		}
	}
	return NULL;
}

as_render_queue* as_rq_create()
{
	as_render_queue* queue = AS_MALLOC_SINGLE(as_render_queue);
	AS_SET_VALID(queue);
	queue->is_running = true;
	queue->thread = as_thread_create(&as_render_queue_thread_run, queue);
	as_thread_set_priority(queue->thread, 2);
	return queue;
}

void as_rq_destroy(as_render_queue* render_queue)
{
	if (AS_IS_INVALID(render_queue)) { return; }
	as_rq_wait_queue(render_queue);
	render_queue->is_running = false;
	as_thread_join(render_queue->thread);
	AS_FREE(render_queue);
}

sz as_rq_get_queue_size(as_render_queue* render_queue)
{
	if (AS_IS_INVALID(render_queue)) { return 0; }
	return render_queue->commands.size;
}

void as_rq_wait_queue(as_render_queue* render_queue)
{
	u32 iteration_counter = 0;
	while (as_rq_get_queue_size(render_queue) > 0 && iteration_counter < AS_RENDER_QUEUE_SIZE)
	{
		sleep_seconds(AS_RENDER_QUEUE_WAIT_TIME);
		//iteration_counter++;
		// wait for queue to be empty
	}
}

void as_rq_submit(as_render_queue* render_queue, void func_ptr(void*), void* arg, const u64 arg_size)
{
	AS_ASSERT(render_queue, "Cannot submit to renderer queue, render_queue is null");
	AS_ASSERT(func_ptr, "Cannot submit to renderer queue, func_ptr is null");

	AS_WAIT_AND_LOCK(render_queue);
	as_render_command command = { 0 };
	command.func_ptr = func_ptr;
	memcpy(command.arg, arg, arg_size);
	AS_ARRAY_INSERT_AT(render_queue->commands, 0, command);
	if (render_queue->commands.size > AS_RENDER_QUEUE_SIZE)
	{
		AS_LOG(LV_ERROR, "Overflowing render queue, please check your update/submission rates, potential crash.");
	}
	AS_SET_UNLOCKED(render_queue);
}

void as_render_start_draw_loop_func(as_render* render) 
{ 
	AS_WAIT_AND_LOCK(render);
	as_render_start_draw_loop(render); 
	AS_SET_UNLOCKED(render);
};
void as_rq_render_start_draw_loop(as_render_queue* render_queue, as_render* render)
{
	 as_rq_submit(render_queue, &as_render_start_draw_loop_func, render, sizeof(render));
}

void as_render_end_draw_loop_func(as_render* render)
{
	AS_WAIT_AND_LOCK(render);
	as_render_end_draw_loop(render);
	AS_SET_UNLOCKED(render);
}
void as_rq_render_end_draw_loop(as_render_queue* render_queue, as_render* render)
{
	as_rq_submit(render_queue, &as_render_end_draw_loop_func, render, sizeof(render));
}

typedef struct as_render_draw_frame_arg
{
	as_render* render;
	void* display_context;
	as_camera* camera;
	as_scene* scene;
} as_render_draw_frame_arg;
void as_render_draw_frame_func(as_render_draw_frame_arg* draw_frame_arg)
{
	AS_WAIT_AND_LOCK(draw_frame_arg->render);
	as_render_draw_frame(draw_frame_arg->render, draw_frame_arg->display_context, draw_frame_arg->camera, draw_frame_arg->scene);
	AS_SET_UNLOCKED(draw_frame_arg->render);
}
void as_rq_render_draw_frame(as_render_queue* render_queue, as_render* render, void* display_context, as_camera* camera,  as_scene* scene)
{
	AS_WAIT_AND_LOCK(render);
	as_render_draw_frame_arg draw_frame_arg = { 0 };
	draw_frame_arg.render = render;
	draw_frame_arg.display_context = display_context;
	draw_frame_arg.scene = scene;
	draw_frame_arg.camera = camera;
	as_rq_submit(render_queue, &as_render_draw_frame_func, &draw_frame_arg, sizeof(draw_frame_arg));
	AS_SET_UNLOCKED(render);
}

void as_render_destroy_func(as_render* render)
{
	AS_WAIT_AND_LOCK(render);
	as_render_destroy(render);
}
void as_rq_render_destroy(as_render_queue* render_queue, as_render* render)
{
	as_rq_submit(render_queue, &as_render_destroy_func, render, sizeof(render));
}

typedef struct as_texture_update_arg
{
	as_texture* texture;
	as_render* render;
} as_texture_update_arg;
void as_texture_update_func(as_texture_update_arg* texture_update_arg)
{
	AS_WAIT_AND_LOCK(texture_update_arg->render);
	as_texture_update(texture_update_arg->render, texture_update_arg->texture);
	AS_SET_UNLOCKED(texture_update_arg->render);
}
void as_rq_texture_update(as_render_queue* render_queue, as_texture* texture, as_render* render)
{
	as_texture_update_arg texture_update_arg = { 0 };
	texture_update_arg.render = render;
	texture_update_arg.texture = texture;
	as_rq_submit(render_queue, &as_texture_update_func, &texture_update_arg, sizeof(texture_update_arg));
}

 typedef struct as_texture_destroy_arg
 {
	 as_texture* texture;
	 as_render* render;
 } as_texture_destroy_arg;
 void as_texture_destroy_func(as_texture_destroy_arg* texture_destroy_arg)
 {
	 AS_WAIT_AND_LOCK(texture_destroy_arg->render);
	 as_texture_destroy(texture_destroy_arg->render, texture_destroy_arg->texture);
	 AS_SET_UNLOCKED(texture_destroy_arg->render);
 }
 void as_rq_texture_destroy(as_render_queue* render_queue, as_render* render, as_texture* texture)
{
	 as_texture_destroy_arg texture_destroy_arg = { 0 };
	 texture_destroy_arg.render = render;
	 texture_destroy_arg.texture = texture;
	 as_rq_submit(render_queue, &as_texture_destroy_func, &texture_destroy_arg, sizeof(texture_destroy_arg));
}

 typedef struct as_shader_set_uniforms_arg
 {
	 as_render* render;
	 as_shader* shader;
	 as_shader_uniforms_32* uniforms;
 } as_shader_set_uniforms_arg;
 void as_shader_set_uniforms_func(as_shader_set_uniforms_arg* shader_set_uniforms_arg)
 {
	 AS_WAIT_AND_LOCK(shader_set_uniforms_arg->render);
	 as_shader_set_uniforms(shader_set_uniforms_arg->render, shader_set_uniforms_arg->shader, shader_set_uniforms_arg->uniforms);
	 AS_SET_UNLOCKED(shader_set_uniforms_arg->render);
 }
 void as_rq_shader_set_uniforms(as_render_queue* render_queue, as_render* render, as_shader* shader, as_shader_uniforms_32* uniforms)
{
	 as_shader_set_uniforms_arg* shader_set_uniforms_arg = AS_MALLOC_SINGLE(as_shader_set_uniforms_arg);
	 shader_set_uniforms_arg->render = render;
	 shader_set_uniforms_arg->shader = shader;
	 shader_set_uniforms_arg->uniforms = uniforms;
	 as_rq_submit(render_queue, &as_shader_set_uniforms_func, shader_set_uniforms_arg, sizeof(shader_set_uniforms_arg));
}

 typedef struct as_shader_update_arg
 {
	 as_render* render;
	 as_shader* shader;
 } as_shader_update_arg;
 void as_shader_update_func(as_shader_update_arg* shader_update_arg)
 {
	 AS_WAIT_AND_LOCK(shader_update_arg->render);
	 as_shader_update(shader_update_arg->render, shader_update_arg->shader);
	 AS_SET_UNLOCKED(shader_update_arg->render);
 }
 void as_rq_shader_update(as_render_queue* render_queue, as_render* render, as_shader* shader)
{
	 as_shader_update_arg shader_update_arg = { 0 };
	 shader_update_arg.render = render;
	 shader_update_arg.shader = shader;
	 as_rq_submit(render_queue, &as_shader_update_func, &shader_update_arg, sizeof(shader_update_arg));
}

 void as_shader_create_graphics_pipeline_func(as_shader* shader)
 {
	 AS_WAIT_AND_LOCK(shader);
	 as_shader_create_graphics_pipeline(shader);
	 AS_SET_UNLOCKED(shader);
 }
 extern void as_rq_shader_recompile(as_render_queue* render_queue, as_shader* shader)
 {
	 if (render_queue)
	 {
		 as_rq_submit(render_queue, &as_shader_create_graphics_pipeline_func, shader, sizeof(shader));
	 }
	 else
	 {
		 as_shader_create_graphics_pipeline(shader);
	 }
 }

 typedef struct as_shader_destroy_arg
 {
	 as_render* render;
	 as_shader* shader;
 } as_shader_destroy_arg;
 void as_shader_destroy_func(as_shader_destroy_arg* shader_destroy_arg)
 {
	 AS_WAIT_AND_LOCK(shader_destroy_arg->render);
	 as_shader_destroy(shader_destroy_arg->render, shader_destroy_arg->shader);
	 AS_SET_UNLOCKED(shader_destroy_arg->render);
 }
 void as_rq_shader_destroy(as_render_queue* render_queue, as_render* render, as_shader* shader)
 {
	 as_shader_destroy_arg shader_destroy_arg = { 0 };
	 shader_destroy_arg.render = render;
	 shader_destroy_arg.shader = shader;
	 as_rq_submit(render_queue, &as_shader_destroy_func, &shader_destroy_arg, sizeof(shader_destroy_arg));
}

 typedef struct as_scene_destroy_arg
 {
	as_render* render;
	as_scene* scene;
 } as_scene_destroy_arg;
 void as_scene_destroy_func(as_scene_destroy_arg* scene_destroy_arg)
 {
	AS_WAIT_AND_LOCK(scene_destroy_arg->render);
	as_scene_destroy(scene_destroy_arg->render, scene_destroy_arg->scene);
	AS_SET_UNLOCKED(scene_destroy_arg->render);
}
void as_rq_scene_destroy(as_render_queue* render_queue, as_render* render, as_scene* scene)
{
	as_scene_destroy_arg scene_destroy_arg = { 0 };
	scene_destroy_arg.render = render;
	scene_destroy_arg.scene = scene;
	as_rq_submit(render_queue, &as_scene_destroy_func, &scene_destroy_arg, sizeof(scene_destroy_arg));
}
