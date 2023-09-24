#include "lua/lua.h"

int dofile(lua_State* L, const char* name);
int report(lua_State* L, int status);
int msghandler(lua_State* L);
int docall(lua_State* L, int narg, int nres);