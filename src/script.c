// handles media key listening & provides the `script` library to lua

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <lua.h>
#include <lauxlib.h>

#include "luautil.h"
#include "script.h"

const char* const event_names[] = { EVENT_MEDIA_PLAY_PAUSE, EVENT_MEDIA_NEXT_TRACK, EVENT_MEDIA_PREVIOUS_TRACK, EVENT_PLAYBACK_COMPLETE, NULL };

#define LUNAR_MEDIA_REGISTRY_KEY "lunar-media"
#define push_registry_table(L) (lua_pushliteral(L, LUNAR_MEDIA_REGISTRY_KEY), lua_rawget(L, LUA_REGISTRYINDEX))

pthread_mutex_t luaStateMutex = PTHREAD_MUTEX_INITIALIZER;
lua_State* global_L;

void script_init() {
  printf("initalizing script\n");
}

int script_raise_event(const char* event) {
  /*push_registry_table(L);
  lua_pushstring(L, event);
  lua_rawget(L, -2);
  if(lua_isfunction(L, -1)) { // may be nil if the handler hasn't been registered yet
    return report(L, docall(L, 0, 0));
  }
  return LUA_OK;*/

  pthread_mutex_lock(&luaStateMutex);
  int nres = 0;
  lua_pushstring(global_L, event); // resume with event name as arg
  int status = lua_resume(global_L, NULL, 1, &nres);
  pthread_mutex_unlock(&luaStateMutex);

  return status;
}

int script_raise_key_event(const char* key) {
  pthread_mutex_lock(&luaStateMutex);
  int nres = 0;
  lua_pushliteral(global_L, EVENT_KEY); // resume with event name, key
  lua_pushstring(global_L, key);
  int status = lua_resume(global_L, NULL, 2, &nres);
  pthread_mutex_unlock(&luaStateMutex);

  return status;
}

int script_on(lua_State* L) {
  luaL_checkoption(L, 1, NULL, event_names);
  if(!lua_isfunction(L, 2)) {
    luaL_typeerror(L, 2, lua_typename(L, LUA_TFUNCTION));
  }
  printf("adding listener for %s: %p\n", lua_tostring(L, 1), lua_topointer(L, 2));

  push_registry_table(L); // top of stack (-1) is our table

  lua_pushvalue(L, 1); // event name as table key
  lua_pushvalue(L, 2); // event handler function as table value
  lua_rawset(L, -3); // table got shifted away from top by 2 values

  return 0;
}

int script_open_lua_lib(lua_State* L) {
  // prepare the registry
  lua_pushliteral(L, LUNAR_MEDIA_REGISTRY_KEY);
  lua_createtable(L, 0, 3); // table with 3 values
  lua_rawset(L, LUA_REGISTRYINDEX);

  luaL_Reg lib[] = {
    { "on", script_on },
    { NULL } // sentinel value to indicate the end of the array
  };

  luaL_newlib(L, lib);
  lua_setglobal(L, "script");

  global_L = L;

  return 0;
}