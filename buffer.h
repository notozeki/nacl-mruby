#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer {
  char *ptr;
  int len;
  int capa;
} buffer;
#define BUFFER_PTR(b) ((b)->ptr)
#define BUFFER_END(b) ((b)->ptr + (b)->len)
#define BUFFER_LEN(b) ((b)->len)
#define BUFFER_CAPA(b) ((b)->capa)

buffer *create_buffer(int capa);
void destroy_buffer(buffer *buf);

int buffer_resize(buffer *buf, int capa);

#endif /* BUFFER_H */
