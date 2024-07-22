// project-wide macros and things
#ifndef lunar_media_h
#define lunar_media_h

#include <SDL.h>

extern char _error_string_buffer[255];
#define msgbox(type, title, ...) sprintf(_error_string_buffer, __VA_ARGS__) ; printf(_error_string_buffer) ; SDL_ShowSimpleMessageBox(type, title, _error_string_buffer, NULL)
#define error(...) msgbox(SDL_MESSAGEBOX_ERROR, "An error occured", __VA_ARGS__)
#define warn(...) msgbox(SDL_MESSAGEBOX_WARNING, "Warning", __VA_ARGS__)

#define LUNAR_MEDIA_NAME "Lunar Media"
#define LUNAR_MEDIA_ID "dev.penguinspy.lunar-media"
#define LUNAR_MEDIA_VERSION "alpha_0"

// calculates the max, but watch out! double evaluation :)
#define MAX(a,b) (((a) > (b) ? (a) : (b)))
#define MIN(a,b) (((a) < (b) ? (a) : (b)))

#define strequal(s1, s2) (strcmp(s1, s2) == 0)

// https://stackoverflow.com/a/12891181
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#endif