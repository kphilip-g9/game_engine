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

funcdef slice<string>
asset_fetch_shader_source(Asset shader, Arena *arena)
{
	// TODO: Karun, implement this
}
