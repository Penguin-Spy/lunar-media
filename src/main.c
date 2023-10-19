/* Copyright © Penguin_Spy 2023
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "vlc/vlc.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "vlc.h"
#include "script.h"
#include "native.h"
#include "luautil.h"
#include "lunar-media.h"

int main(int argc, char* argv[]) {

  for(int i = 0; i < argc; i++) {
    printf("  argv[%i]: %s\n", i, argv[i]);
  }

  printf("hello world? 2\n");

  //SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);
  //GetModuleHandle(NULL);


  vlc_init();
  script_init();
  native_init();

  printf("lua version: %s\n", LUA_RELEASE);
  lua_State* L = luaL_newstate();

  luaL_openlibs(L);     // open standard libraries
  vlc_open_lua_lib(L);  // add our vlc library
  script_open_lua_lib(L);

  dofile(L, "main.lua");


  // windows shenanigans & event loop
  pthread_t thread_id;
  printf("b4 thread\n");
  pthread_create(&thread_id, NULL, native_exec, NULL);
  printf("thread id: %lli\n", thread_id);
  pthread_join(thread_id, NULL);
  printf("after thread join\n");

  //native_exec(NULL);

  while(true) {
    sleep(1);
    printf(".");
  }

  // Stop playing
  vlc_stop(L);
  vlc_release();

  return 0;
}
