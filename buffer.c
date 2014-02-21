#include <stdlib.h>

#include "buffer.h"

#define BUFFER_CAPA_MIN 1024

buffer *
create_buffer(int capa)
{
  buffer *buf;

  if (capa < BUFFER_CAPA_MIN) {
    capa = BUFFER_CAPA_MIN;
  }

  buf = (buffer *)malloc(sizeof(buffer));
  if (!buf) {
    return NULL;
  }
  buf->ptr = (char *)malloc(capa);
  if (!buf->ptr) {
    free(buf);
    return NULL;
  }
  buf->len = 0;
  buf->capa = capa;

  return buf;
}

void
destroy_buffer(buffer *buf)
{
  if (buf) {
    free(buf->ptr);
    free(buf);
  }
}

int
buffer_resize(buffer *buf, int capa)
{
  char *ptr;

  if (capa < buf->len) {
    return 0;
  }
  
  ptr = (char *)realloc(buf->ptr, capa);
  if (!ptr) {
    return 0;
  }
  buf->ptr = ptr;
  buf->capa = capa;

  return 1;
}
