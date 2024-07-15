// handles os-specific functionality (media key listening, media player display, etc.)

#include <stdio.h>
#include <windows.h>
#include <pthread.h>

#include "script.h"
#include "lunar-media.h"


// convert microsoft's virtual key codes to strings
char* vkToKey(DWORD vk) {
  switch(vk) {
    case VK_BACK: return "backspace";
    case VK_TAB:  return "tab";
    //case VK_CLEAR:
    case VK_RETURN:  return "enter";
    case VK_SHIFT:   return "shift";
    case VK_CONTROL: return "ctrl";
    case VK_MENU:    return "alt";
    case VK_PAUSE:   return "pause";
    case VK_CAPITAL: return "capslock";
    case VK_ESCAPE:  return "esc";

    case VK_SPACE: return " ";
    case VK_PRIOR: return "pageup";
    case VK_NEXT:  return "pagedown";
    case VK_END:   return "end";
    case VK_HOME:  return "home";
    case VK_LEFT:  return "left";
    case VK_UP:    return "up";
    case VK_RIGHT: return "right";
    case VK_DOWN:  return "down";
    case VK_INSERT: return "ins";
    case VK_DELETE: return "del";
    case VK_HELP:   return "help";

    case 0x30: return "0";
    case 0x31: return "1";
    case 0x32: return "2";
    case 0x33: return "3";
    case 0x34: return "4";
    case 0x35: return "5";
    case 0x36: return "6";
    case 0x37: return "7";
    case 0x38: return "8";
    case 0x39: return "9";

    case 0x41: return "A";
    case 0x42: return "B";
    case 0x43: return "C";
    case 0x44: return "D";
    case 0x45: return "E";
    case 0x46: return "F";
    case 0x47: return "G";
    case 0x48: return "H";
    case 0x49: return "I";
    case 0x4A: return "J";
    case 0x4B: return "K";
    case 0x4C: return "L";
    case 0x4D: return "M";
    case 0x4E: return "N";
    case 0x4F: return "O";
    case 0x50: return "P";
    case 0x51: return "Q";
    case 0x52: return "R";
    case 0x53: return "S";
    case 0x54: return "T";
    case 0x55: return "U";
    case 0x56: return "V";
    case 0x57: return "W";
    case 0x58: return "X";
    case 0x59: return "Y";
    case 0x5A: return "Z";

    default: return NULL;
  }
}



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

void* native_2(void* arg) {
  //lua_State* L = arg;
  printf("in thread\n");

  DWORD cNumRead, fdwMode, i;
  INPUT_RECORD irInBuf[128];

  HANDLE hStdin;
  DWORD fdwSaveOldMode;
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  if(hStdin == INVALID_HANDLE_VALUE)
    error("GetStdHandle");

  // Save the current input mode, to be restored on exit.
  if(!GetConsoleMode(hStdin, &fdwSaveOldMode))
    error("GetConsoleMode");

  // Enable the window and mouse input events.
  fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
  if(!SetConsoleMode(hStdin, fdwMode))
    error("SetConsoleMode");

  while(ReadConsoleInput(
    hStdin,      // input buffer handle
    irInBuf,     // buffer to read into
    128,         // size of read buffer
    &cNumRead)) { // number of records read

    for(i = 0; i < cNumRead; i++) {
      switch(irInBuf[i].EventType) {
        case KEY_EVENT: // keyboard input

          // no UTF-8 support, i really tried i promise.

          KEY_EVENT_RECORD ker = irInBuf[i].Event.KeyEvent;
          if(ker.bKeyDown)
            printf("key pressed: %u,%u\n", ker.wVirtualKeyCode, ker.uChar.UnicodeChar);
          else printf("key released: %u,%u\n", ker.wVirtualKeyCode, ker.uChar.UnicodeChar);

          char* str = vkToKey(ker.wVirtualKeyCode);
          printf("?: %s\n", str);
          script_raise_key_event(str);



          /*if(ker.wVirtualKeyCode == 81) { // q
            // Restore input mode on exit.
            SetConsoleMode(hStdin, fdwSaveOldMode);
            return;
          }*/
          break;

        case MOUSE_EVENT: // mouse input
          printf("mouse: %lu", irInBuf[i].Event.MouseEvent.dwEventFlags);
          break;

        case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
          //ResizeEventProc();
          WINDOW_BUFFER_SIZE_RECORD wbsr = irInBuf[i].Event.WindowBufferSizeEvent;
          printf("Console screen buffer is %d columns by %d rows.\n", wbsr.dwSize.X, wbsr.dwSize.Y);
          break;

        case FOCUS_EVENT:  // disregard focus events

        case MENU_EVENT:   // disregard menu events
          break;

        default:
          error("Unknown event type %u", irInBuf[i].EventType);
          break;
      }
    }
  }

  // Restore input mode on exit.
  SetConsoleMode(hStdin, fdwSaveOldMode);
  printf("key read thread closing");

  return NULL;
}

// entrypoint of the event loop thread
void native_exec(void* arg) {
  printf("inside thread exec\n");
  //lua_State* L = arg;

  pthread_t thread_id;
  printf("b4 thread\n");
  pthread_create(&thread_id, NULL, native_2, NULL/*L*/);
  printf("thread id: %lli\n", thread_id);
  //pthread_join(thread_id, NULL);
  //printf("after thread join\n");


  //HWND hwnd = GetConsoleWindow();
  MSG msg = { 0 };
  BOOL bret;
  while((bret = GetMessage(&msg, NULL, 0, 0)) != 0) {
    printf("message: %u | ", msg.message);
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    if(bret == -1) {
      printf("get message error: %llu", msg.wParam);
      break;
    }
    if(msg.message == WM_HOTKEY) {
      //printf(" got hotkey %llu", msg.wParam);
      switch(msg.wParam) {
        case 0:
          script_raise_event(EVENT_MEDIA_PLAY_PAUSE);
          //printf("got hotkey play/pause");
          break;
        case 1:
          script_raise_event(EVENT_MEDIA_NEXT_TRACK);
          //printf("got hotkey next track");
          break;
        case 2:
          script_raise_event(EVENT_MEDIA_PREVIOUS_TRACK);
          //printf("got hotkey previous track");
          break;
      }
    } else if(msg.message == WM_KEYDOWN) {
      printf("wParam: %llu", msg.wParam);
    } else if(msg.message == WM_CHAR) {
      printf("char wParam: %llu", msg.wParam);
    } else if(msg.message == WM_APPCOMMAND) {
      printf("got appcommand");
      switch(GET_APPCOMMAND_LPARAM(msg.lParam)) {
        case APPCOMMAND_MEDIA_PLAY_PAUSE:
          printf(" play/pause");
          break;
        default:
          printf("%i", GET_APPCOMMAND_LPARAM(msg.lParam));
      }
    } else {
      //
    }
    printf("\n");
  }

  error("native thread loop exited: %lu", GetLastError());
}