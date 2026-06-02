#include "engine.h"

#include <math.h>

funcdef void
entry_point(slice<string> args)
{
	os_init();
	defer(os_deinit());

	OS_Handle window = os_open_window(S("engine"));
	defer(os_close_window(window));

	gfx_init();
	defer(gfx_deinit());

	Draw_Data draw_data = {};

	gfx_init_draw_data(&draw_data, Draw::Triangles);
	defer(gfx_deinit_draw_data(&draw_data));

	vec2 size = os_window_size(window);

	draw_data.camera.scale = 1.0f;


	f32 time = 0.0f;

	while(!os_window_should_close(window))
	{
		os_prepare_frame(window);

		vec2 resolution = os_window_size(window);
		draw_data.camera.offset = {
			resolution.x * 0.5f,
			resolution.y * 0.5f
		};

		gfx_begin(&draw_data, resolution);

		const int GRID_SIZE = 10;
		const f32 TILE_SIZE = 64.0f;

		for (int y = -GRID_SIZE; y < GRID_SIZE; ++y)
		{
			for (int x = -GRID_SIZE; x < GRID_SIZE; ++x)
			{
				color8 color = ((x + y) & 1) ? Hex(0x458588ff) : Hex(0x83a598ff);

				f32 min_x = (f32)x * TILE_SIZE;
				f32 min_y = (f32)y * TILE_SIZE;

				f32 max_x = min_x + TILE_SIZE;
				f32 max_y = min_y + TILE_SIZE;

				gfx_push_quad({ min_x, min_y }, { max_x, max_y }, color);
			}
		}

		gfx_end();
	}
}
