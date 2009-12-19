#ifndef __UTILS_H__
#define __UTILS_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>

#define BUG_ON(cond)

#define panic(x...) do \
  { \
    fprintf(stderr, "=============PANIC PANIC PANIC===========\n"); \
    fprintf(stderr, x); \
    fprintf(stderr, "=========================================\n"); \
    exit(1); \
  } while(0)

#define panic_if(cond, x...) \
  do { if ((cond)) panic(x); } while(0)

static inline char *xstrdup(const char *s)
{
  char *out = strdup(s);

  panic_if(!out, "strdup failed");

  return out;
}

static inline void *xmalloc(size_t sz)
{
  void *out = malloc(sz);

  panic_if(!out, "malloc failed");
  memset(out, 0, sz);

  return out;
}

static inline void *xrealloc(void *ptr, size_t sz)
{
  void *out = realloc(ptr, sz);

  panic_if(!out, "malloc failed");

  return out;
}

#define xsnprintf(buf, size, fmt, x...) do { \
    int r = snprintf(buf, size, fmt, x); \
    panic_if(r < 0 || r >= (int)(size), "snprintf failed for %s with %d\n", fmt, r); \
} while(0)

TTF_Font *read_and_alloc_font(const char *path, int pt_size);

const char **get_file_list(const char *base_dir, const char *exts[]);

void *sdl_surface_to_png(SDL_Surface *src, size_t *out_sz);

#endif /* __UTILS_H__ */