#ifndef gui_h
#define gui_h

#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>

typedef enum { TEXT, BUTTON, FLOW } gui_element_type;

typedef struct {
  SDL_Texture* texture;
  SDL_Rect position;
} gui_text_element;

typedef struct {
  SDL_Texture* texture;
  SDL_Rect position;
} gui_button_element;

typedef struct {
  bool direction; // true for horizontal
} gui_flow_element;

typedef struct gui_element_t gui_element;
typedef struct gui_element_t {
  gui_element_type type;
  gui_element* children;
  gui_element* sibling;
  union {
    gui_text_element text;
    gui_button_element button;
    gui_flow_element flow;
  };
} gui_element;


int gui_init();
int gui_quit();
void gui_get_window_size(int* w, int* h);
TTF_Font* gui_load_font(char* filename);

void gui_add_child(gui_element* parent, gui_element* element);
gui_element* gui_create_text_element(TTF_Font* font, char* string);
gui_element* gui_create_flow_element(bool direction);

int gui_on_window_resized(void* userdata, SDL_Event* event);
void gui_render();


extern gui_element* gui_root;

#endif