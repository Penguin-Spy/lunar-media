#ifndef gui_h
#define gui_h

#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>

typedef enum { TEXT, BUTTON, FLOW } gui_element_type;
#define GUI_ELEMENT_ROOT "theme"
#define GUI_ELEMENT_HEAD "head"
#define GUI_ELEMENT_FONT "font"
#define GUI_ELEMENT_SPRITE "sprite"
#define GUI_ELEMENT_BODY "body"
#define GUI_ELEMENT_FLOW "flow"
#define GUI_ELEMENT_LABEL "label"
#define GUI_ELEMENT_IMG "img"

#define GUI_ATTRIBUTE_NAME "name"
#define GUI_ATTRIBUTE_SRC "src"
#define GUI_ATTRIBUTE_SIZE "size"
#define GUI_ATTRIBUTE_WIDTH "width"
#define GUI_ATTRIBUTE_HEIGHT "height"

#define GUI_ATTRIBUTE_DIRECTION "direction"
#define GUI_ATTRIBUTE_DIRECTION_HORIZONTAL "horizontal"
#define GUI_ATTRIBUTE_DIRECTION_VERTICAL "vertical"

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
  int width;
  int height;
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
TTF_Font* gui_load_font(const char* filename, int size);
void gui_load_document(char* filename);

void gui_add_child(gui_element* parent, gui_element* element);
gui_element* gui_create_text_element(const char* string);
gui_element* gui_create_flow_element(bool direction);

int gui_on_window_resized(void* userdata, SDL_Event* event);
void gui_render();


extern gui_element* gui_root;

#endif