/* Copyright © Penguin_Spy 2023
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include "vlc/vlc.h"
#include "lua/lua.h"
#include <windows.h>

int main(int argc, char* argv[]) {
  WINBOOL success;

  for(int i = 0; i < argc; i++) {
    printf("  argv[%i]: %s\n", i, argv[i]);
  }
  if(argc != 2) {
    printf("usage: lunar-media.exe <path>");
    return EXIT_FAILURE;
  }

  printf("hello world? 2\n");
  printf("libvlc version: %s (compiler: %s)\n", libvlc_get_version(), libvlc_get_compiler());
  printf("lua version: %s\n", LUA_RELEASE);

  //SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);

  GetModuleHandle(NULL);
  success = RegisterHotKey(NULL, 0, 0, VK_MEDIA_PLAY_PAUSE);
  if(!success) {
    printf("hotkey VK_MEDIA_PLAY_PAUSE failed: %lu", GetLastError());
  }
  success = RegisterHotKey(NULL, 1, 0, VK_MEDIA_NEXT_TRACK);
  if(!success) {
    printf("hotkey VK_MEDIA_NEXT_TRACK failed: %lu", GetLastError());
  }
  success = RegisterHotKey(NULL, 2, 0, VK_MEDIA_PREV_TRACK);
  if(!success) {
    printf("hotkey VK_MEDIA_PREV_TRACK failed: %lu", GetLastError());
  }

  /* Load the VLC engine */
  libvlc_instance_t* inst = libvlc_new(0, NULL);
  if(inst == NULL) {
    fprintf(stderr, "uh oh! instance was NULL: %s\n", libvlc_errmsg());
  }
  libvlc_set_user_agent(inst, "Lunar media", "lunar-media/alpha-0"); // name shows up in windows "Volume mixer"
  libvlc_set_app_id(inst, "dev.penguinspy.lunar-media", "alpha-0", "media");

  /* Create a new item */
  libvlc_media_t* m = libvlc_media_new_path(inst, argv[1]);

  if(m == NULL) {
    fprintf(stderr, "uh oh! media was NULL: %s\n", libvlc_errmsg());
  }

  /* Create a media player playing environement */
  //mp = libvlc_media_player_new_from_media(m);
  libvlc_media_player_t* mp = libvlc_media_player_new(inst);

  if(mp == NULL) {
    fprintf(stderr, "uh oh! media player was NULL: %s\n", libvlc_errmsg());
  }

  libvlc_media_player_set_media(mp, m);

  /* No need to keep the media now */
  libvlc_media_release(m);

  libvlc_audio_set_volume(mp, 50);

  /* play the media_player */
  printf("b4 play");
  libvlc_media_player_play(mp);
  printf(" a5ter play\n");

  MSG msg = { 0 };
  while(GetMessageW(&msg, NULL, 0, 0) != 0) {
    printf("\nmessage: %u", msg.message);
    if(msg.message == WM_HOTKEY) {
      printf(" got hotkey %llu", msg.wParam);
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
  libvlc_media_player_stop(mp);

  // Free the media_player
  libvlc_media_player_release(mp);

  libvlc_release(inst);

  return 0;
}
