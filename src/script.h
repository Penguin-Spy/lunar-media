
#define LUNAR_MEDIA_REGISTRY_KEY "lunar-media"
#define push_registry_table(L) (lua_pushliteral(L, LUNAR_MEDIA_REGISTRY_KEY), lua_rawget(L, LUA_REGISTRYINDEX))

#define EVENT_MEDIA_PLAY_PAUSE "media_play_pause"
#define EVENT_MEDIA_NEXT_TRACK "media_next_track"
#define EVENT_MEDIA_PREVIOUS_TRACK "media_previous_track"

void script_init();

int script_open_lua_lib(lua_State* L);

int script_raise_event(lua_State* L, const char* event);
