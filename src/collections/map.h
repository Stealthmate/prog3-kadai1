#ifndef COLLECTIONS_MAP_H
#define COLLECTIONS_MAP_H

#include "../master.h"

typedef long int key;
typedef long int (*hash_f)(void *item);
typedef struct map map;
typedef struct _map_iter* map_iter;

typedef struct {
  key k;
  void *v;
} map_entry;

map *map_init();
void map_destroy(map *m);
int map_is_empty(map *m);

int map_put(map *m, key k, void *v);
size_t map_size(map *m);

map_iter map_begin(map *m);
map_iter map_end(map *m);
map_iter map_lookup(map *m, key k);
void *map_lookup_value(map *m, key k);

int map_pop(map_iter mi, map_entry **me);
int map_pop_key(map *m, key k, map_entry **me);
int map_erase(map_iter mi);
int map_erase_key(map *m, key k);

int map_iter_incr(map_iter mi);

int map_iter_eq(map_iter lhs, map_iter rhs);
void map_iter_cp_assign(map_iter lhs, map_iter rhs);
void map_iter_mv_assign(map_iter lhs, map_iter rhs);

map_entry *map_iter_deref(map_iter mi);

void map_iter_destroy(map_iter mi);

#define MAP_FOREACH(i, m) {                                             \
  map_iter i;                                                           \
  map_iter __end = map_end(m);                                          \
  for(i = map_begin(m); !map_iter_eq(i, __end); map_iter_incr(i))

#define MAP_FOREACH_END(i)                      \
  map_iter_destroy(i);                          \
  map_iter_destroy(__end);                      \
  }

#endif
