
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <ezxml.h>

#include "gui.h"
#include "lunar-media.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

gui_element* gui_root = NULL;

//static TTF_Font* fonts;
TTF_Font* default_font;
TTF_Font* fallback_font;

int colors_r[] = { 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF };
int colors_g[] = { 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0x00, 0x00 };
int colors_b[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF };

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
    1400, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

TTF_Font* gui_load_font(const char* filename, int size) {
  TTF_Font* font = TTF_OpenFont(filename, size);
  if(!font) {
    error("Unable to load font: '%s'!\n"
      "SDL2_ttf Error: %s\n", filename, TTF_GetError());
    return NULL;
  }
  return font;
}

void gui_get_window_size(int* w, int* h) {
  SDL_GetWindowSize(window, w, h);
}

int gui_on_window_resized(void* UNUSED(userdata), SDL_Event* event) {
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

gui_element* gui_create_text_element(const char* string) {
  SDL_Color textColor           = { 0x00, 0x00, 0x00, 0xFF };
  SDL_Color textBackgroundColor = { 0xFF, 0xFF, 0xFF, 0xFF };
  SDL_Rect position;

  // check that the requested font provides every character in the string, and fall back to rendering the whole string in GNU Unifont if not.
  bool codepoint_provided = true;
  for(unsigned i = 0; i < strlen(string); i++) {
    uint32_t codepoint = 0;
    if(!(string[i] & 0x80)) { // 0xxxxxxx
      codepoint = string[i];
    } else if((string[i] & 0xE0) == 0xC0) { // 110xxxxx 10xxxxxx
      codepoint = ((string[i] & 0x1F) << 6) | (string[i+1] & 0x3F);
      i += 1;
    } else if((string[i] & 0xF0) == 0xE0) { // 1110xxxx 10xxxxxx 10xxxxxx
      codepoint = (((string[i] & 0x0F) << 12) | ((string[i+1] & 0x3F) << 6) | (string[i+2] & 0x3F));
      i += 2;
    } else if((string[i] & 0xF8) == 0xF0) { // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
      codepoint = (((string[i] & 0x07) << 18) | ((string[i+1] & 0x3F) << 12) | ((string[i+2] & 0x3F) << 6) | (string[i+3] & 0x3F));
      i += 3;
    } else {
      error("invalid utf-8 string?: '%02x %02x %02x %02x'\n", string[i], string[i+1], string[i+2], string[i+3]);
    }
    if(TTF_GlyphIsProvided32(default_font, codepoint) == 0) {
      codepoint_provided = false;
      break;
    }
  }
  // TODO: ideally, render the string in segments (as many chars in requested font as available, sections in unifont where not available)
  SDL_Surface* textSurface = TTF_RenderUTF8_Shaded(codepoint_provided ? default_font : fallback_font, string, textColor, textBackgroundColor);
  if(!textSurface) {
    error("Unable to render text surface!\nSDL2_ttf Error: %s\n", TTF_GetError());
    return NULL;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
  if(!texture) {
    error("Unable to create texture from rendered text!\nSDL2 Error: %s\n", SDL_GetError());
    return NULL;
  }

  position.w = textSurface->w;
  position.h = textSurface->h;
  SDL_FreeSurface(textSurface);
  position.x = 0;
  position.y = 0;

  gui_element* element = malloc(sizeof(gui_element));
  element->type = TEXT;
  element->text.texture = texture;
  element->text.position = position; // TODO: position should be local & go out of scope, so this should break but it doesn't. figure out why
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
  gui_element* element = NULL;

  // common attributes
  int width = -1, height = -1;
  const char* width_attr = ezxml_attr(item, GUI_ATTRIBUTE_WIDTH);
  if(width_attr) {
    width = strtoul(width_attr, NULL, 10);
    if(errno == ERANGE || width == 0) {
      warn("invalid width '%s'\n", width_attr);
    }
  }
  const char* height_attr = ezxml_attr(item, GUI_ATTRIBUTE_HEIGHT);
  if(height_attr) {
    height = strtoul(height_attr, NULL, 10);
    if(errno == ERANGE || height == 0) {
      warn("invalid height '%s'\n", height_attr);
    }
  }

  if(strequal(item->name, GUI_ELEMENT_FLOW) || strequal(item->name, GUI_ELEMENT_BODY)) {
    printf("as flow ");
    bool direction = false; // true for horizontal
    const char* direction_attr = ezxml_attr(item, GUI_ATTRIBUTE_DIRECTION);
    if(direction_attr) {
      direction = strequal(direction_attr, GUI_ATTRIBUTE_DIRECTION_HORIZONTAL);
    }
    printf("with '%i', %i, %i\n", direction, width, height);
    element = gui_create_flow_element(direction);
    item = item->child;
    while(item != NULL) {
      gui_add_child(element, load_xml_item(item));
      item = item->ordered;
    }
  } else if(strequal(item->name, GUI_ELEMENT_LABEL)) {
    printf("as label with '%s'\n", item->txt);
    element = gui_create_text_element(item->txt);
  } else if(strequal(item->name, GUI_ELEMENT_IMG)) {
    printf("as img");
    const char* src = ezxml_attr(item, GUI_ATTRIBUTE_SRC);
    printf(" with '%s'\n", src ? src : "NULL");
    element = gui_create_text_element(src);
  } else if(strequal(item->name, "section")) {
    printf("as section\n");
    element = gui_create_text_element("SECTION PLACEHOLDER");
  } else {
    error("failed to load document, unknown element '%s'\n", item->name);
    return NULL;
  }
  element->width = width;
  element->height = height;
  return element;
}

void load_document_head(ezxml_t head) {
  ezxml_t font = ezxml_child(head, GUI_ELEMENT_FONT);
  /*int font_count = 0;
  while(font != NULL) {
    font_count++;
    font = font->next;
  }*/
  //fonts = malloc(font_count * sizeof(fonts));
  while(font != NULL) {
    printf("loading font ");
    const char* name = ezxml_attr(font, GUI_ATTRIBUTE_NAME);
    const char* src = ezxml_attr(font, GUI_ATTRIBUTE_SRC);
    int size = strtoul(ezxml_attr(font, GUI_ATTRIBUTE_SIZE), NULL, 10);
    if(errno == ERANGE || size == 0) {
      error("invalid font size '%s'\n", ezxml_attr(font, GUI_ATTRIBUTE_SIZE));
      continue;
    }
    printf("'%s' from '%s' in %i\n", name, src, size);
    if(strequal(name, "default")) {
      default_font = gui_load_font(src, size);
    } else if(strequal(name, "fallback")) {
      fallback_font = gui_load_font(src, size);
    }
    font = font->next;
  }

}

void gui_load_document(char* filename) {
  ezxml_t theme = ezxml_parse_file(filename);
  if(theme == NULL) {
    error("failed to parse document: %s\n", filename);
    return;
  }

  ezxml_t head = theme->child;
  if(!strequal(head->name, GUI_ELEMENT_HEAD)) {
    error("the first child of the THEME must be the HEAD element, found '%s'\n", head->name);
    return;
  }
  load_document_head(head);

  ezxml_t body = head->ordered;
  if(!strequal(body->name, GUI_ELEMENT_BODY)) {
    error("the second child of the THEME must be the BODY element, found '%s'\n", body->name);
    return;
  }
  if(body->ordered != NULL) {
    error("unexpected child of the THEME: '%s'\n", body->ordered->name);
    return;
  }

  gui_add_child(gui_root, load_xml_item(body));
}

/**
 * renders all of a flow's children
 * @param available_area  input: `x,y` top left, `w,h` width/height of maximum area the element can use up;
                          output: used area: `w,h` how much space the element actually used of its alloted space
 */
void render_flow(gui_element* flow, SDL_Rect* available_area, int level) {
  // x or y (depending on flow dir) are adjusted as elements are rendered
  // width and height are adjusted to contain the whole area of the flow as elements are rendered
  int x = available_area->x;
  int y = available_area->y;
  int used_width = 0;
  int used_height = 0;

  //SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, SDL_ALPHA_OPAQUE);
  //SDL_RenderDrawRect(renderer, available_area);

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
        SDL_Rect child_flow_area;
        child_flow_area.x = x + 1;
        child_flow_area.y = y + 1;
        child_flow_area.w = available_area->w - (x - available_area->x) - 2; // available area minus (used area: current pos - inital pos) minus margins
        child_flow_area.h = available_area->h - (y - available_area->y) - 2;
        render_flow(cur, &child_flow_area, level + 1);
        if(cur->width > 0) {
          width = available_area->w * cur->width / 100;
        } else {
          width = child_flow_area.w + 2;
        }
        if(cur->height > 0) {
          height = available_area->h * cur->height / 100;
        } else {
          height = child_flow_area.h + 2;
        }
        break;
      }
    }

    if(direction) { // if horizontal, add the element's width to the current X pos, and save the highest Y size
      x += width;
      used_height = MAX(used_height, height);
    } else {  // if vertical, add the element's height to the current Y pos, and save the highest X size
      y += height;
      used_width = MAX(used_width, width);
    }

    cur = cur->sibling;
  }

  // update the component of size that wasn't updated in the above loop
  if(direction) {
    used_width = x - available_area->x;
  } else {
    used_height = y - available_area->y;
  }

  // use the area rect to pass the used area to the caller
  available_area->w = used_width;
  available_area->h = used_height;

  // render debugging outline of flow's used area
  SDL_SetRenderDrawColor(renderer, colors_r[level], colors_g[level], colors_b[level], SDL_ALPHA_OPAQUE);
  SDL_RenderDrawRect(renderer, available_area);
  SDL_SetRenderDrawColor(renderer, colors_r[level], colors_g[level], colors_b[level], 0x7F);
  //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  //SDL_RenderFillRect(renderer, available_area);
}

void gui_render() {
  // white background
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);

  SDL_Rect area;
  area.x = 0; area.y = 0;
  gui_get_window_size(&area.w, &area.h);
  render_flow(gui_root, &area, 1);

  SDL_RenderPresent(renderer);
}
