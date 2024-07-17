#ifndef script_h
#define script_h

#include <lua.h>

#define EVENT_MEDIA_PLAY_PAUSE "media_play_pause"
#define EVENT_MEDIA_NEXT_TRACK "media_next_track"
#define EVENT_MEDIA_PREVIOUS_TRACK "media_previous_track"
#define EVENT_PLAYBACK_COMPLETE "playback_complete"
#define EVENT_KEY "key"

void script_init();

int script_open_lua_lib(lua_State* L);

int script_raise_event(const char* event);
int script_raise_key_event(const char* key);

#endif