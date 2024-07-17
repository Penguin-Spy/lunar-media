
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "gui.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

gui_element_list* elements = NULL;

int gui_init() {
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL2 could not be initialized!\n"
      "SDL2 Error: %s\n", SDL_GetError());
    return 0;
  }

  TTF_Init();

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
  if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) {
    printf("SDL can not disable compositor bypass!\n");
    return 0;
  }
#endif

  window = SDL_CreateWindow("lunar media",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    600, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if(!window) {
    printf("Window could not be created!\n"
      "SDL_Error: %s\n", SDL_GetError());
    return gui_quit();
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer) {
    printf("Renderer could not be created!\n"
      "SDL_Error: %s\n", SDL_GetError());
    return gui_quit();
  }

  elements = malloc(sizeof(gui_element_list));
  elements->cur = NULL;
  elements->next = NULL;

  return 1;
}

int gui_quit() {
  if(renderer) {
    SDL_DestroyRenderer(renderer);
  }

  if(window) {
    SDL_DestroyWindow(window);
  }

  TTF_Quit();
  SDL_Quit();
  return 0;
}

void gui_get_window_size(int* w, int* h) {
  SDL_GetWindowSize(window, w, h);
}

TTF_Font* gui_load_font(char* filename) {
  TTF_Font* font = TTF_OpenFont(filename, 40);
  if(!font) {
    printf("Unable to load font: '%s'!\n"
      "SDL2_ttf Error: %s\n", filename, TTF_GetError());
    return NULL;
  }
  return font;
}

void append_element(gui_text_element* element) {
  gui_element_list* list = elements;
  while(list->cur != NULL) {
    list = list->next;
  }
  list->cur = element;
  list->next = malloc(sizeof(gui_element_list));
  list->next->cur = NULL;
  list->next->next = NULL;
}

gui_text_element* gui_create_text_element(TTF_Font* font, char* string, int x, int y) {
  SDL_Color textColor           = { 0x00, 0x00, 0x00, 0xFF };
  SDL_Color textBackgroundColor = { 0xFF, 0xFF, 0xFF, 0xFF };
  SDL_Texture* texture = NULL;
  SDL_Rect position;

  // TODO: check that the requested font provides every character in the string, and fall back to rendering the whole string in GNU Unifont if not.
  SDL_Surface* textSurface = TTF_RenderUTF8_Shaded(font, string, textColor, textBackgroundColor);
  if(!textSurface) {
    printf("Unable to render text surface!\n"
      "SDL2_ttf Error: %s\n", TTF_GetError());
  } else {
    texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if(!texture) {
      printf("Unable to create texture from rendered text!\n"
        "SDL2 Error: %s\n", SDL_GetError());
      return NULL;
    }

    // Get text dimensions
    position.w = textSurface->w;
    position.h = textSurface->h;

    SDL_FreeSurface(textSurface);
  }

  position.x = x;
  position.y = y;

  gui_text_element* text = malloc(sizeof(gui_text_element));
  text->texture = texture;
  text->position = position;
  append_element(text);
  return text;
}

void gui_render() {
  // white background
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);

  gui_element_list* list = elements;
  while(list->cur != NULL) {
    SDL_RenderCopy(renderer, list->cur->texture, NULL, &list->cur->position);
    list = list->next;
  }

  SDL_RenderPresent(renderer);
}
