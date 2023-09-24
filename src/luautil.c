// helpful functions for calling Lua functions
// these are mostly yoinked from lua.c (the standalone interpreter)

#include <stdio.h>

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "luautil.h"

int dofile(lua_State* L, const char* name) {
  int status = luaL_loadfile(L, name);
  if(status == LUA_OK) status = docall(L, 0, LUA_MULTRET);
  return report(L, status);
}

/*
** Check whether 'status' is not OK and, if so, prints the error
** message on the top of the stack. It assumes that the error object
** is a string, as it was either generated by Lua or by 'msghandler'.
*/
int report(lua_State* L, int status) {
  if(status != LUA_OK) {
    const char* msg = lua_tostring(L, -1);
    fprintf(stderr, "lua: %s\n", msg);
    lua_pop(L, 1);  /* remove message */
  }
  return status;
}

/*
** Message handler used to run all chunks
*/
int msghandler(lua_State* L) {
  const char* msg = lua_tostring(L, 1);
  if(msg == NULL) {  /* is error object not a string? */
    if(luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
      lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
      return 1;  /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)",
        luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
  return 1;  /* return the traceback */
}

/*
** Interface to 'lua_pcall', which sets appropriate message function
** and C-signal handler. Used to run all chunks.
*/
int docall(lua_State* L, int narg, int nres) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, msghandler);  /* push message handler */
  lua_insert(L, base);  /* put it under function and args */
  //globalL = L;  /* to be available to 'laction' */
  //setsignal(SIGINT, laction);  /* set C-signal handler */
  status = lua_pcall(L, narg, nres, base);
  //setsignal(SIGINT, SIG_DFL); /* reset C-signal handler */
  lua_remove(L, base);  /* remove message handler from the stack */
  return status;
}