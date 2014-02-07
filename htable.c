#include <stdint.h>
#include <stdlib.h>

#include "htable.h"

#define HTABLE_DEFAULT_SLOT_SIZE (UINT32_C(11))

struct htable_entry {
  uint64_t key;
  void *value;
  struct htable_entry *next;
};

struct htable {
  htable_type *type;
  uint32_t num_entries;
  uint32_t num_slots;
  struct htable_entry **slots;
};

static inline int
do_compare(htable *table, uint64_t key1, uint64_t key2)
{
  return ((*table->type->compare))(key1, key2);
}

static inline uint32_t
do_hash(htable *table, uint64_t key)
{
  return (*(table->type->hash))(key, table->num_slots);
}

static inline void
do_free(htable *table, void *value)
{
  ((*table->type->free))(value);
}


htable *
create_htable(htable_type *type)
{
  htable *table;
  uint32_t size;

  size = HTABLE_DEFAULT_SLOT_SIZE;

  table = (htable *)malloc(sizeof(htable));
  if (!table) {
    return NULL;
  }
  table->type = type;
  table->num_entries = 0;
  table->num_slots = size;
  table->slots =
    (struct htable_entry **)calloc(size, sizeof(struct htable_entry *));
  if (!table->slots) {
    free(table);
    return NULL;
  }

  return table;
}

static inline void
free_entry(htable *table, struct htable_entry *entry)
{
  do_free(table, entry->value);
  free(entry);
}

void
destroy_htable(htable *table)
{
  struct htable_entry *pos, *next;
  int i;

  for (i = 0; i < table->num_slots; i++) {
    pos = table->slots[i];
    while (pos != NULL) {
      next = pos->next;
      free_entry(table, pos);
      pos = next;
    }
  }
  free(table->slots);
  free(table);
}


int
htable_size(htable *table)
{
  return table->num_entries;
}

static inline struct htable_entry *
find_entry(htable *table, uint64_t key, uint32_t hash_val)
{
  struct htable_entry *pos;

  for (pos = table->slots[hash_val]; pos != NULL; pos = pos->next) {
    if (do_compare(table, pos->key, key) == 0) {
      return pos;
    }
  }
  return NULL;
}

static inline struct htable_entry *
create_entry(uint64_t key, void *value)
{
  struct htable_entry *entry;

  entry = (struct htable_entry *)malloc(sizeof(struct htable_entry));
  if (!entry) {
    return NULL;
  }
  entry->key = key;
  entry->value = value;
  entry->next = NULL;

  return entry;
}

void *
htable_insert_no_free(htable *table, uint64_t key, void *value)
{
  struct htable_entry *entry;
  uint32_t hash_val;
  void *old_value = NULL;

  hash_val = do_hash(table, key);
  entry = find_entry(table, key, hash_val);

  if (entry) {
    old_value = entry->value;
    entry->value = value;
  }
  else {
    entry = create_entry(key, value);
    entry->next = table->slots[hash_val];
    table->slots[hash_val] = entry;
    table->num_entries++;
  }

  return old_value; /* NULL if no entry for key */
}

int
htable_insert(htable *table, uint64_t key, void *value)
{
  void *old_value;

  old_value = htable_insert_no_free(table, key, value);
  if (old_value) {
    do_free(table, old_value);
    return 1;
  }
  else {
    return 0;
  }
}

void *
htable_find(htable *table, uint64_t key)
{
  struct htable_entry *entry;
  uint32_t hash_val;

  hash_val = do_hash(table, key);
  entry = find_entry(table, key, hash_val);

  if (entry) {
    return entry->value;
  }
  else {
    return NULL;
  }
}

void *
htable_remove_no_free(htable *table, uint64_t key)
{
  struct htable_entry *pos, *to_be_free;
  uint32_t hash_val;
  void *retval = NULL;

  hash_val = do_hash(table, key);

  pos = table->slots[hash_val]; 
  if (pos == NULL) {
    return NULL;
  }
  else if (do_compare(table, pos->key, key) == 0) {
    to_be_free = pos;
    table->slots[hash_val] = pos->next;
    retval = to_be_free->value;
    free(to_be_free);
    table->num_entries--;
    return retval;
  }

  for (; pos->next != NULL; pos = pos->next) {
    if (do_compare(table, pos->next->key, key) == 0) {
      to_be_free = pos->next;
      pos->next = pos->next->next;
      retval = to_be_free->value;
      free(to_be_free);
      table->num_entries--;
      return retval;
    }
  }

  return NULL;
}

int
htable_remove(htable *table, uint64_t key)
{
  void *value;

  value = htable_remove_no_free(table, key);
  if (value) {
    do_free(table, value);
    return 1;
  }
  else {
    return 0;
  }
}
