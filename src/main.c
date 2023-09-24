/* Copyright © Penguin_Spy 2023
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "vlc/vlc.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "vlc.h"
#include "script.h"
#include "luautil.h"

#define error(s, ...) fprintf(stderr, s, __VA_ARGS__)

int main(int argc, char* argv[]) {

  for(int i = 0; i < argc; i++) {
    printf("  argv[%i]: %s\n", i, argv[i]);
  }

  printf("hello world? 2\n");

  //SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);
  //GetModuleHandle(NULL);


  vlc_init();
  script_init();

  printf("lua version: %s\n", LUA_RELEASE);
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);     // open standard libraries
  vlc_open_lua_lib(L);  // add our vlc library
  script_open_lua_lib(L);

  dofile(L, "main.lua");


  // windows shenanigans & event loop
  // TODO: do this cross platform? put this in it's own file

  WINBOOL success = RegisterHotKey(NULL, 0, 0, VK_MEDIA_PLAY_PAUSE);
  if(!success) {
    error("hotkey VK_MEDIA_PLAY_PAUSE failed: %lu", GetLastError());
  }
  success = RegisterHotKey(NULL, 1, 0, VK_MEDIA_NEXT_TRACK);
  if(!success) {
    error("hotkey VK_MEDIA_NEXT_TRACK failed: %lu", GetLastError());
  }
  success = RegisterHotKey(NULL, 2, 0, VK_MEDIA_PREV_TRACK);
  if(!success) {
    error("hotkey VK_MEDIA_PREV_TRACK failed: %lu", GetLastError());
  }

  MSG msg = { 0 };
  while(GetMessageW(&msg, NULL, 0, 0) != 0) {
    //printf("\nmessage: %u", msg.message);
    if(msg.message == WM_HOTKEY) {
      //printf(" got hotkey %llu", msg.wParam);
      switch(msg.wParam) {
        case 0:
          script_raise_event(L, EVENT_MEDIA_PLAY_PAUSE);
          break;
        case 1:
          script_raise_event(L, EVENT_MEDIA_NEXT_TRACK);
          break;
        case 2:
          script_raise_event(L, EVENT_MEDIA_PREVIOUS_TRACK);
          break;
      }
    }
    if(msg.message == WM_APPCOMMAND) {
      printf(" got appcommand");
      switch(GET_APPCOMMAND_LPARAM(msg.lParam)) {
        case APPCOMMAND_MEDIA_PLAY_PAUSE:
          printf(" play/pause");
          break;
        default:
          printf("%i", GET_APPCOMMAND_LPARAM(msg.lParam));
      }
    }
  }

  // Stop playing
  vlc_stop(L);
  vlc_release();

  return 0;
}
