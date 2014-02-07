#ifndef HTABLE_H
#define HTABLE_H

#include <stdint.h>

typedef struct htable htable;
typedef struct {
  int (*compare)(uint64_t key1, uint64_t key2);
  uint32_t (*hash)(uint64_t key, uint32_t modulus);
  void (*free)(void *value);
} htable_type;


htable *create_htable(htable_type *type);
void destroy_htable(htable *table);

int htable_size(htable *table);
int htable_insert(htable *table, uint64_t key, void *value);
void *htable_insert_no_free(htable *table, uint64_t key, void *value);
void *htable_find(htable *table, uint64_t key);
int htable_remove(htable *table, uint64_t key);
void *htable_remove_no_free(htable *table, uint64_t key);

#endif /* HTABLE_H */
