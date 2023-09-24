// Initalizes the libVLC instance and provides the `vlc` library to lua

#include <stdio.h>
#include <stdlib.h>

#include "vlc/vlc.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"

// the libVLC instance (there should only ever be one).
libvlc_instance_t* inst = NULL;
// the libVLC media player
libvlc_media_player_t* mp = NULL;

int vlc_init() {
  if(inst != NULL) {
    fprintf(stderr, "libVLC is already initalized!");
    return 1;
  }

  printf("initalizing libVLC version: %s (build: %s)\n", libvlc_get_version(), libvlc_get_compiler());

  // Load the VLC engine
  inst = libvlc_new(0, NULL);
  if(inst == NULL) {
    fprintf(stderr, "uh oh! instance was NULL: %s\n", libvlc_errmsg());
  }
  libvlc_set_user_agent(inst, "Lunar media", "lunar-media/alpha-0"); // name shows up in windows "Volume mixer"
  libvlc_set_app_id(inst, "dev.penguinspy.lunar-media", "alpha-0", "media");

  // Create the media player
  mp = libvlc_media_player_new(inst);
  if(mp == NULL) {
    fprintf(stderr, "uh oh! media player was NULL: %s\n", libvlc_errmsg());
  }

  return 0;
}

void vlc_release() {
  libvlc_media_player_release(mp);
  libvlc_release(inst);
}

int vlc_play(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);

  libvlc_media_t* m = libvlc_media_new_path(inst, path);
  if(m == NULL) {
    fprintf(stderr, "uh oh! media was NULL: %s\n", libvlc_errmsg());
  }

  libvlc_media_player_set_media(mp, m);
  libvlc_media_release(m);

  libvlc_media_player_play(mp);

  return 0;
}

int vlc_is_playing(lua_State* L) {
  int is_playing = libvlc_media_player_is_playing(mp);
  lua_pushboolean(L, is_playing);
  return 1;
}

int vlc_pause(lua_State* L) {
  libvlc_media_player_set_pause(mp, 1);
  return 0;
}
int vlc_resume(lua_State* L) {
  libvlc_media_player_set_pause(mp, 0);
  return 0;
}
int vlc_stop(lua_State* L) {
  libvlc_media_player_stop(mp);
  return 0;
}

int vlc_set_volume(lua_State* L) {
  int volume = (int)luaL_checkinteger(L, 1);
  libvlc_audio_set_volume(mp, volume);
  return 0;
}

int vlc_open_lua_lib(lua_State* L) {
  luaL_Reg lib[] = {
    { "play",       vlc_play },
    { "pause",      vlc_pause },
    { "resume",     vlc_resume },
    { "is_playing", vlc_is_playing },
    { "stop",       vlc_stop },
    { "set_volume", vlc_set_volume },
    { NULL } // sentinel value to indicate the end of the array
  };

  luaL_newlib(L, lib);
  lua_setglobal(L, "vlc");

  return 0;
}
