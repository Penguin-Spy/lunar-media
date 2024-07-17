#ifndef vlc_h
#define vlc_h

#include <lua.h>

int vlc_init();
void vlc_release();

int vlc_open_lua_lib(lua_State* L);

int vlc_stop(lua_State* L);

/*
int vlc_play(lua_State* L);
int vlc_pause(lua_State* L);
int vlc_resume(lua_State* L);
int vlc_set_volume(lua_State* L);
*/

#endif