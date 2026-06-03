#include "engine.h"

#if DEBUG_BUILD

typedef u64 Asset_TimeStamp;
struct Asset_Record {
	Asset_TimeStamp last_edit;
};

funcdef string asset__get_path(Asset a) 
{

#define Asset_Entry(_t, _p) case Asset::_t : return S(_p)
	
switch (a) {
	Asset_Entry(Sprite_Shader, "./shaders/sprite.glsl");
	Asset_Entry(Screen_Shader, "./shaders/screen.glsl");
	
	default: assert(false);	
	}

#undef Asset_Entry
}


#endif


global struct 
{
#if DEBUG_BUILD
	Asset_Record records[(u64) Asset::Count];
#endif

	list<Sprite> sprites;
} asset_ctx;


funcdef void
assets_init()
{
#if DEBUG_BUILD

	u64 asset_count = (u64) Asset::Count;
	for (u64 i=0; i<asset_count; ++i) {
		
	}

#endif
}

funcdef void
assets_deinit()
{

}

// ~karun
// Loads a .glsl file that contains both vertex and fragment shader
// separated by #vert and #frag directives.
// Returns a slice of 2 strings: [0] = vertex source, [1] = fragment source.
funcdef slice<string>
asset_fetch_shader_source(Asset shader, Arena *arena)
{
	assert(shader == Asset::Sprite_Shader || shader == Asset::Screen_Shader);

	string path = asset__get_path(shader);
	bytes  data = os_load_entire_file(arena, path);
	string str  = string_strip(string_from_bytes(data));

	slice<string> result = alloc_slice(arena, string, 2);

	Temp t = temp_begin(scratch());
	defer(temp_end(t));

	slice<string> lines = string_as_lines(scratch(), str);

	u64 vs_begin = lines.len + 1; // just initialize to a huge value
	u64 fs_begin = lines.len + 1;

	for (u64 i = 0; i < lines.len; ++i) {
		if (string_equal(lines[i], S("#vert"))) {
			assert(vs_begin > lines.len);
			vs_begin = i;
		}

		if (string_equal(lines[i], S("#frag"))) {
			assert(fs_begin > lines.len);
			fs_begin = i;
		}
	}

	// make sure we actually found both markers
	assert(vs_begin <= lines.len && "missing #vert in shader file");
	assert(fs_begin <= lines.len && "missing #frag in shader file");

	const u8 *str_end = str.raw + str.len;

	if (vs_begin < fs_begin) {
		const u8 *vert_begin = lines[vs_begin + 1].raw;
		const u8 *frag_begin = lines[fs_begin + 1].raw;

		result[0] = string { vert_begin, (u64)(lines[fs_begin].raw - vert_begin) };
		result[1] = string { frag_begin, (u64)(str_end - frag_begin) };
	} else {
		const u8 *frag_begin = lines[fs_begin + 1].raw;
		const u8 *vert_begin = lines[vs_begin + 1].raw;

		result[1] = string { frag_begin, (u64)(lines[vs_begin].raw - frag_begin) };
		result[0] = string { vert_begin, (u64)(str_end - vert_begin) };
	}

	return result;
}