// project-wide macros and things
#ifndef lunar_media_h
#define lunar_media_h

#define error(...) fprintf(stderr, __VA_ARGS__)

// https://stackoverflow.com/a/12891181
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#endif