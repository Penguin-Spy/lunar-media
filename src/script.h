#include "lua/lua.h"

#define EVENT_MEDIA_PLAY_PAUSE "media_play_pause"
#define EVENT_MEDIA_NEXT_TRACK "media_next_track"
#define EVENT_MEDIA_PREVIOUS_TRACK "media_previous_track"
#define EVENT_PLAYBACK_COMPLETE "playback_complete"

void script_init();

int script_open_lua_lib(lua_State* L);

int script_raise_event(lua_State* L, const char* event);
