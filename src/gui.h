
#ifndef gui_h
#define gui_h

#include <SDL.h>
#include <SDL_ttf.h>


typedef struct {
  SDL_Texture* texture;
  SDL_Rect position;
} gui_text_element;

typedef struct gui_element_list_t gui_element_list;

struct gui_element_list_t {
  gui_text_element* cur;
  gui_element_list* next;
};

int gui_init();
int gui_quit();
void gui_get_window_size(int* w, int* h);
TTF_Font* gui_load_font(char* filename);
gui_text_element* gui_create_text_element(TTF_Font* font, char* string, int x, int y);

void gui_render();

#endif