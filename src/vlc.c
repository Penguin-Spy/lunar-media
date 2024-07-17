// Initalizes the libVLC instance and provides the `vlc` library to lua

#include <stdio.h>
#include <stdlib.h>

#include <vlc/vlc.h>
#include <lua.h>
#include <lauxlib.h>

#include "script.h"
#include "lunar-media.h"

// the libVLC instance (there should only ever be one).
libvlc_instance_t* inst = NULL;
// the libVLC media player
libvlc_media_player_t* mp = NULL;
libvlc_event_manager_t* em = NULL;  // and its event manager

int vlc_init() {
  if(inst != NULL) {
    error("libVLC is already initalized!");
    return 1;
  }

  printf("initalizing libVLC version: %s (build: %s)\n", libvlc_get_version(), libvlc_get_compiler());

  // Load the VLC engine
  inst = libvlc_new(0, NULL);
  if(inst == NULL) {
    error("uh oh! instance was NULL: %s\n", libvlc_errmsg());
  }
  libvlc_set_user_agent(inst, LUNAR_MEDIA_NAME, LUNAR_MEDIA_NAME "/" LUNAR_MEDIA_VERSION); // name shows up in windows "Volume mixer"
  libvlc_set_app_id(inst, LUNAR_MEDIA_ID, LUNAR_MEDIA_VERSION, "media");

  // Create the media player
  mp = libvlc_media_player_new(inst);
  if(mp == NULL) {
    error("uh oh! media player was NULL: %s\n", libvlc_errmsg());
  }

  // Get the event manager (events are registered once the lua lib is opened)
  em = libvlc_media_player_event_manager(mp);

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
    error("uh oh! media was NULL: %s\n", libvlc_errmsg());
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

int vlc_pause(lua_State* UNUSED(L)) {
  libvlc_media_player_set_pause(mp, 1);
  return 0;
}
int vlc_resume(lua_State* UNUSED(L)) {
  libvlc_media_player_set_pause(mp, 0);
  return 0;
}
int vlc_stop(lua_State* UNUSED(L)) {
  libvlc_media_player_stop(mp);
  return 0;
}

int vlc_set_volume(lua_State* L) {
  int volume = (int)luaL_checkinteger(L, 1);
  libvlc_audio_set_volume(mp, volume);
  return 0;
}


void vlc_handle_end_reached(const struct libvlc_event_t* UNUSED(p_event), void* UNUSED(p_data)) {
  script_raise_event(EVENT_PLAYBACK_COMPLETE);
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

  // begin listening for events
  libvlc_event_attach(em, libvlc_MediaPlayerEndReached, vlc_handle_end_reached, L);

  return 0;
}
