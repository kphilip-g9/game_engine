#include <stddef.h>

#include "engine.h"
#include "vendor/glad.h"

#define MAX_VERTICES 2048


static struct {
	Arena *persist;
	OS_TimeStamp last_frame_time;
	f32 delta_time;

	vec2 resolution;

	Draw_Data *data;
} gfx_ctx;

funcdef void
gfx__flush(Draw_Data *data)
{
	if (data->vertices.len == 0 || data->indices.len == 0) {
		return;
	}

	glBindVertexArray(data->vao);

	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	glBufferSubData(
		GL_ARRAY_BUFFER,
		0,
		data->vertices.len * sizeof(Vertex),
		data->vertices.raw
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->ebo);
	glBufferSubData(
		GL_ELEMENT_ARRAY_BUFFER,
		0,
		data->indices.len * sizeof(u16),
		data->indices.raw
	);

	glUseProgram(data->shader_id);

	glUniform2f(
		glGetUniformLocation(data->shader_id, "u_camera_position"),
		data->camera.position.x,
		data->camera.position.y
	);

	glUniform2f(
		glGetUniformLocation(data->shader_id, "u_camera_offset"),
		data->camera.offset.x,
		data->camera.offset.y
	);

	glUniform1f(
		glGetUniformLocation(data->shader_id, "u_camera_scale"),
		data->camera.scale
	);

	glUniform2f(
		glGetUniformLocation(data->shader_id, "u_resolution"),
		gfx_ctx.resolution.x,
		gfx_ctx.resolution.y
	);

	GLenum mode = GL_TRIANGLES;
	if (data->primitive == Draw::Lines) {
		mode = GL_LINES;
	}

	glDrawElements(
		mode,
		(GLsizei)data->indices.len,
		GL_UNSIGNED_SHORT,
		nullptr
	);

	data->vertices.len = 0;
	data->indices.len  = 0;
}

funcdef void
gfx_init()
{
	if (!gladLoadGLLoader((GLADloadproc)os_get_gl_proc_address())) {
		fprintf(stderr, "failed to load opengl");
		return;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gfx_ctx.last_frame_time = os_time_now();
	gfx_ctx.persist = arena_make(MB(4));
}

funcdef void
gfx_deinit()
{
	// for now does nothing
}

funcdef u32
gfx__compile_shader(int type, string src)
{
	u32 id = glCreateShader(type);

	const char *cstr = (char *)src.raw;
	int len = (int)src.len;

	glShaderSource(id, 1, &cstr, &len);
	glCompileShader(id);

	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (!success) {
		char log[1024];
		glGetShaderInfoLog(id, sizeof(log), 0, log);
		printf("Shader compile error:\n%s\n", log);
		return 0;
	}

	return id;
}

funcdef void
gfx_init_draw_data(Draw_Data *data, Draw type)
{
	data->primitive = type;

	slice<Vertex> vertices = alloc_slice(gfx_ctx.persist, Vertex, MAX_VERTICES);
	slice<u16>    indices  = alloc_slice(gfx_ctx.persist, u16, MAX_VERTICES);

	data->vertices = list_make(vertices);
	data->indices  = list_make(indices);

	data->camera.position = {0, 0};
	data->camera.offset   = {0, 0};
	data->camera.scale    = 1.0f;

	glGenVertexArrays(1, &data->vao);
	glGenBuffers(1, &data->vbo);
	glGenBuffers(1, &data->ebo);

	glBindVertexArray(data->vao);

	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_VERTICES * sizeof(u16), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texcoords));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)offsetof(Vertex, color));

	// temporary shader loading, soon we will make it better
	{
		slice<string> sources = asset_fetch_shader_source(Asset::Sprite_Shader, gfx_ctx.persist);
 
		u32 vs_id = gfx__compile_shader(GL_VERTEX_SHADER,   sources[0]);
		u32 fs_id = gfx__compile_shader(GL_FRAGMENT_SHADER, sources[1]);
 
		assert(vs_id && fs_id);
 
		data->shader_id = glCreateProgram();
		glAttachShader(data->shader_id, vs_id);
		glAttachShader(data->shader_id, fs_id);
		glLinkProgram(data->shader_id);
 
		glDeleteShader(vs_id);
		glDeleteShader(fs_id);
 
		glUseProgram(data->shader_id);
	}
}

funcdef void
gfx_deinit_draw_data(Draw_Data *data)
{
    glDeleteVertexArrays(1, &data->vao);
    glDeleteBuffers(1, &data->vbo);
    glDeleteBuffers(1, &data->ebo);
}

funcdef void
gfx_begin(Draw_Data *data, vec2 resolution)
{
	OS_TimeStamp curr = os_time_now();

	gfx_ctx.delta_time = (f32)os_time_diff(
		gfx_ctx.last_frame_time,
		curr
	).seconds;

	gfx_ctx.last_frame_time = curr;

	gfx_ctx.data = data;
	gfx_ctx.resolution = resolution;

	glViewport(0, 0, (s32) resolution.x, (s32) resolution.y);

	glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

funcdef void
gfx_end()
{
	if (gfx_ctx.data) {
		gfx__flush(gfx_ctx.data);
	}

	gfx_ctx.data = nullptr;
}

funcdef void
gfx_push_quad(vec2 min, vec2 max, color8 color)
{
	Draw_Data *data = gfx_ctx.data;
	assert(data);

	if (data->vertices.len + 4 > data->vertices.capacity ||
		data->indices.len + 6 > data->indices.capacity)
	{
		gfx__flush(data);
	}

	u16 base = (u16)data->vertices.len;

	Vertex vertices[4] = {
		{
			{ min.x, min.y, 0.0f },
			{ 0.0f, 0.0f },
			color,
		},
		{
			{ max.x, min.y, 0.0f },
			{ 1.0f, 0.0f },
			color,
		},
		{
			{ max.x, max.y, 0.0f },
			{ 1.0f, 1.0f },
			color,
		},
		{
			{ min.x, max.y, 0.0f },
			{ 0.0f, 1.0f },
			color,
		},
	};

	u16 indices[6] = {
		(u16) (base + 0),
		(u16) (base + 1),
		(u16) (base + 2),

		(u16) (base + 2),
		(u16) (base + 3),
		(u16) (base + 0),
	};

	append_slice(&data->vertices, { vertices, 4 } );
	append_slice(&data->indices,  { indices, 6 } );
}

funcdef f32 
delta_time()
{
	return gfx_ctx.delta_time;
}
