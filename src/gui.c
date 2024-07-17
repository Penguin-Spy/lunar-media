
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <ezxml.h>

#include "gui.h"
#include "lunar-media.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

gui_element* gui_root = NULL;
TTF_Font* default_font;

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

  window = SDL_CreateWindow(LUNAR_MEDIA_NAME,
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

  gui_root = malloc(sizeof(gui_element));
  gui_root->type = FLOW;
  gui_root->flow.direction = false;
  gui_root->children = NULL;
  gui_root->sibling = NULL;

  SDL_AddEventWatch(gui_on_window_resized, NULL);

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

TTF_Font* gui_load_font(char* filename) {
  TTF_Font* font = TTF_OpenFont(filename, 40);
  if(!font) {
    printf("Unable to load font: '%s'!\n"
      "SDL2_ttf Error: %s\n", filename, TTF_GetError());
    return NULL;
  }
  // TODO: allow themes to specify the font(s) to use, including default font
  default_font = font;
  return font;
}

void gui_get_window_size(int* w, int* h) {
  SDL_GetWindowSize(window, w, h);
}

int gui_on_window_resized(void* userdata, SDL_Event* event) {
  if(event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
    // TODO: technically this isn't allowed (rendering in the event filter) but it doesn't crash (yet) so ¯\_(ツ)_/¯
    gui_render();
  }
  return 1;
}

void gui_add_child(gui_element* parent, gui_element* element) {
  gui_element* cur = parent->children;
  if(cur == NULL) { // parent has no children, add element as first child
    parent->children = element;
  } else {          // parent has 1 or more children, find end of list and add element as sibling
    while(cur->sibling != NULL) {
      cur = cur->sibling;
    }
    cur->sibling = element;
  }
  element->sibling = NULL;
}

gui_element* gui_create_text_element(TTF_Font* font, char* string) {
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

  position.x = 0;
  position.y = 0;

  gui_element* element = malloc(sizeof(gui_element));
  element->type = TEXT;
  element->text.texture = texture;
  element->text.position = position;
  return element;
}

gui_element* gui_create_flow_element(bool direction) {
  gui_element* element = malloc(sizeof(gui_element));
  element->type = FLOW;
  element->flow.direction = direction;
  element->children = NULL;
  element->sibling = NULL;
  return element;
}

gui_element* load_xml_item(ezxml_t item) {
  printf("loading '%s' ", item->name);
  if(strequal(item->name, GUI_ELEMENT_FLOW) || strequal(item->name, GUI_ELEMENT_ROOT)) {
    printf("as flow ");
    bool direction = false;
    for(int i = 0; item->attr[i] != NULL; i += 2) {
      printf("%s=%s ", item->attr[i], item->attr[i+1]);
      if(strequal(item->attr[i], GUI_ATTRIBUTE_DIRECTION)) {
        if(strequal(item->attr[i+1], GUI_ATTRIBUTE_DIRECTION_HORIZONTAL)) {
          direction = true;
        }
      }
    }
    printf("\n");
    gui_element* flow = gui_create_flow_element(direction);
    item = item->child;
    while(item != NULL) {
      gui_add_child(flow, load_xml_item(item));
      item = item->ordered;
    }
    return flow;
  } else if(strequal(item->name, GUI_ELEMENT_LABEL)) {
    printf("as label with '%s'\n", item->txt);
    return gui_create_text_element(default_font, item->txt);
  } else {
    error("failed to load document, unknown element '%s'\n", item->name);
  }
  return NULL;
}

void gui_load_document(char* filename) {
  ezxml_t theme = ezxml_parse_file(filename);
  if(theme == NULL) {
    error("failed to parse document: %s\n", filename);
    return;
  }

  gui_root = load_xml_item(theme);
}

// renders all of a flow's children
// TODO: this whole function feels unelegant, but it works just fine and doing it any other way is messier;
void render_flow(gui_element* flow, int x, int y, SDL_Point* size) {
  // x or y (depending on flow dir) are adjusted as elements are rendered
  // width and height are adjusted to contain the whole area of the flow as elements are rendered
  size->x = 0;
  size->y = 0;

  // start outline in the top left of the flow
  SDL_Rect outline;
  outline.x = x;
  outline.y = y;

  // margins pt.1
  x++; y++;

  gui_element* cur = flow->children;
  bool direction = flow->flow.direction;
  while(cur != NULL) {
    int width = 0, height = 0;
    switch(cur->type) {
      case TEXT: {
        cur->text.position.x = x;
        cur->text.position.y = y;
        SDL_RenderCopy(renderer, cur->text.texture, NULL, &cur->text.position);
        width = cur->text.position.w;
        height = cur->text.position.h;
        break;
      }
      case BUTTON: {
        printf("rendering button‽, ");
        break;
      }
      case FLOW: {
        SDL_Point child_flow_size;
        render_flow(cur, x, y, &child_flow_size);
        width = child_flow_size.x;
        height = child_flow_size.y;
        break;
      }
    }

    if(direction) { // if horizontal, add the element's width to the current X pos, and save the highest Y size
      x += width;
      size->y = MAX(size->y, height);
    } else {  // if vertical, add the element's height to the current Y pos, and save the highest X size
      y += height;
      size->x = MAX(size->x, width);
    }

    cur = cur->sibling;
  }

  // update the component of size that wasn't updated in the above loop
  if(direction) {
    size->x = x - outline.x;
  } else {
    size->y = y - outline.y;
  }

  // margins pt.2
  size->x += 2; size->y += 2;

  // set outline to contain the whole size of the flow
  outline.w = size->x;
  outline.h = size->y;
  // and render it
  SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawRect(renderer, &outline);
}

void gui_render() {
  // white background
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);

  SDL_Point size;
  render_flow(gui_root, 0, 0, &size);

  SDL_RenderPresent(renderer);
}
