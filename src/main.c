/* Copyright © Penguin_Spy 2024
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 */

#include <stdio.h>
#include <stdbool.h>
#include <locale.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "vlc.h"
#include "script.h"
#include "gui.h"
#include "luautil.h"
#include "lunar-media.h"

int main(int argc, char* argv[]) {
  for(int i = 0; i < argc; i++) {
    printf("  argv[%i]: %s\n", i, argv[i]);
  }

  vlc_init();
  script_init();
  gui_init();
  TTF_Font* font = gui_load_font("Montserrat-Regular.ttf");

  gui_load_document("theme.xml");

  int window_width, window_height;
  gui_get_window_size(&window_width, &window_height);

  printf("lua version: %s\n", LUA_RELEASE);
  lua_State* L = luaL_newstate();

  luaL_openlibs(L);     // open standard libraries
  vlc_open_lua_lib(L);  // add our vlc library
  script_open_lua_lib(L);

  int status = luaL_loadfile(L, "main.lua"); // puts the main chunk on the stack as a function
  if(status == LUA_OK) {
    int nres = 0;
    status = lua_resume(L, NULL, 0, &nres); // leaves the main function on the stack
    printf("resume: %i, %i\n", status, nres);
  }
  report(L, status);

  bool quit = false, resized = true;
  while(!quit) {
    SDL_Event e;
    SDL_WaitEvent(&e);
    switch(e.type) {
      case SDL_QUIT: quit = true;
        break;
      case SDL_WINDOWEVENT: {
        if(e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          resized = true;
          printf("r.");
        }
        break;
      }
    }

    if(resized) {
      gui_get_window_size(&window_width, &window_height);
      resized = false;
      printf("resized!\n");
    }

    // TODO: don't re-render if nothing's changed, it causes up to 30% GPU usage unnecessarily
    gui_render();
  }

  gui_quit();

// Stop playing
  vlc_stop(L);
  vlc_release();

  return 0;
}
