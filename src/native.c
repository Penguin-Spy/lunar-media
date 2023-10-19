// handles os-specific functionality (media key listening, media player display, etc.)

#include <stdio.h>
#include <windows.h>

#include "script.h"
#include "lunar-media.h"

void native_init() {
  printf("initalizing native event stuff\n");

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
}

// entrypoint of the event loop thread
void native_exec(void* UNUSED(arg)) {
  printf("inside thread exec\n");

  MSG msg = { 0 };
  while(GetMessageW(&msg, NULL, 0, 0) != 0) {
    printf("message: %u | ", msg.message);
    if(msg.message == WM_HOTKEY) {
      //printf(" got hotkey %llu", msg.wParam);
      switch(msg.wParam) {
        case 0:
          //script_raise_event(L, EVENT_MEDIA_PLAY_PAUSE);
          printf("got hotkey play/pause");
          break;
        case 1:
          //script_raise_event(L, EVENT_MEDIA_NEXT_TRACK);
          printf("got hotkey next track");
          break;
        case 2:
          //script_raise_event(L, EVENT_MEDIA_PREVIOUS_TRACK);
          printf("got hotkey previous track");
          break;
      }
    }
    if(msg.message == WM_APPCOMMAND) {
      printf("got appcommand");
      switch(GET_APPCOMMAND_LPARAM(msg.lParam)) {
        case APPCOMMAND_MEDIA_PLAY_PAUSE:
          printf(" play/pause");
          break;
        default:
          printf("%i", GET_APPCOMMAND_LPARAM(msg.lParam));
      }
    }
    printf("\n");
  }

  error("native thread loop exited: %lu", GetLastError());
}