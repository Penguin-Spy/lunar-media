// project-wide macros and things
#ifndef lunar_media_h
#define lunar_media_h

#define error(...) fprintf(stderr, __VA_ARGS__)

#define LUNAR_MEDIA_NAME "Lunar Media"
#define LUNAR_MEDIA_ID "dev.penguinspy.lunar-media"
#define LUNAR_MEDIA_VERSION "alpha_0"

// calculates the max, but watch out! double evaluation :)
#define MAX(a,b) (((a) > (b) ? (a) : (b)))
#define MIN(a,b) (((a) < (b) ? (a) : (b)))

// https://stackoverflow.com/a/12891181
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#endif