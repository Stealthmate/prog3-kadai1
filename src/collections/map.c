#include "map.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "linked_list.h"


#define INIT_SIZE 17

struct map {
  int size;
  linked_list **buckets;
  int *bucket_sizes;
};

struct _map_iter {
  map *m;
  int bucket;
  linked_list_iter lli;
};

int _k2b(map *m, key k) {
  return k % m->size;
}

int _me_eq_k(void *lhs, void *rhs) {
  return ((map_entry*) lhs)->k == ((map_entry*) rhs)->k;
}

map *map_init() {
  map *m = (map*) malloc(sizeof(map));

  m->size = INIT_SIZE;
  m->buckets = (linked_list**) malloc(sizeof(linked_list*) * m->size);
  m->bucket_sizes = (int*) malloc(sizeof(int) * m->size);
  for(int i=0;i<m->size;i++) {
    m->buckets[i] = linked_list_init();
    m->bucket_sizes[i] = 0;
  }

  return m;
}

void map_destroy(map *m) {
  for(int i=0;i<m->size;i++) {
    linked_list_destroy(m->buckets[i]);
  }
  free(m->buckets);
  free(m->bucket_sizes);
  free(m);
}

int map_is_empty(map *m) {
  for(int i=0;i<m->size;i++) {
    if(m->bucket_sizes[i] > 1) return 0;
  }

  return 1;
}

int map_put(map *m, key k, void *v) {
  map_entry *me = (map_entry*) malloc(sizeof(map_entry));
  if(me == NULL) return COLLECTION_NOMEM;

  me->k = k;
  me->v = v;

  int bucket = _k2b(m, me->k);
  int res = linked_list_append(m->buckets[bucket], me);
  if(res != COLLECTION_OK) return res;
  m->bucket_sizes[bucket] += 1;

  return COLLECTION_OK;
}

int map_size(map *m) {
  int s = 0;
  for(int i=0;i<m->size;i++) s += m->bucket_sizes[i];
  return s;
}

void _fast_forward(map_iter mi) {
  linked_list_iter end = linked_list_end(mi->m->buckets[mi->bucket]);
  while(mi->bucket < mi->m->size - 1)  {
    if(!linked_list_iter_eq(mi->lli, end)) break;

    mi->bucket += 1;
    linked_list_iter_mv_assign(mi->lli, linked_list_begin(mi->m->buckets[mi->bucket]));
    linked_list_iter_mv_assign(end, linked_list_end(mi->m->buckets[mi->bucket]));
  }

  linked_list_iter_destroy(end);
}

map_iter map_begin(map *m) {
  map_iter mi = (map_iter) malloc(sizeof(struct _map_iter));
  mi->m = m;
  mi->bucket = 0;
  mi->lli = linked_list_begin(m->buckets[mi->bucket]);
  _fast_forward(mi);
  return mi;
}

map_iter map_end(map *m) {
  map_iter mi = (map_iter) malloc(sizeof(struct _map_iter));
  mi->m = m;
  mi->bucket = m->size - 1;
  mi->lli = linked_list_end(m->buckets[mi->bucket]);
  return mi;
}

map_iter map_lookup(map *m, key k) {
  map_iter mi = (map_iter) malloc(sizeof(struct _map_iter));
  map_entry me;
  me.k = k;

  mi->bucket = _k2b(m, k);
  mi->lli = linked_list_find(m->buckets[mi->bucket], &me, _me_eq_k);
  linked_list_iter end = linked_list_end(m->buckets[mi->bucket]);

  if(linked_list_iter_eq(mi->lli, end)) {
    linked_list_iter_destroy(mi->lli);
    mi = map_end(m);
  }

  linked_list_iter_destroy(end);

  return mi;
}

void *map_lookup_value(map *m, key k) {
  map_iter mi = map_lookup(m, k);
  map_entry *me = map_iter_deref(mi);
  map_iter_destroy(mi);
  return me->v;
}

int map_pop(map_iter mi, map_entry **me) {
  map_iter end = map_end(mi->m);
  if(map_iter_eq(mi, end)) {
    map_iter_destroy(end);
    return COLLECTION_ILLEGAL_ARG;
  }

  int res = linked_list_pop(mi->lli, (void**) me);
  if(res != COLLECTION_OK) return COLLECTION_ILLEGAL_ARG;
  mi->m->bucket_sizes[mi->bucket] -= 1;
  _fast_forward(mi);

  return COLLECTION_OK;
}

int map_erase(map_iter mi) {
  map_entry *me;
  int res = map_pop(mi, &me);
  free(me);
  return res;
}

int map_erase_key(map *m, key k) {
  map_iter mi = map_lookup(m, k);
  int res = map_erase(mi);
  map_iter_destroy(mi);
  return res;
}

int map_iter_incr(map_iter mi) {
  int res = linked_list_iter_incr(mi->lli);
  _fast_forward(mi);
  return res;
}

int map_iter_eq(map_iter lhs, map_iter rhs) {
  return (lhs->m == rhs->m)
    && (lhs->bucket == rhs->bucket)
    && linked_list_iter_eq(lhs->lli, rhs->lli);
}

void map_iter_cp_assign(map_iter lhs, map_iter rhs) {
  lhs->m = rhs->m;
  lhs->bucket = rhs->bucket;
  linked_list_iter_cp_assign(lhs->lli, rhs->lli);
}

void map_iter_mv_assign(map_iter lhs, map_iter rhs) {
  map_iter_cp_assign(lhs, rhs);
  map_iter_destroy(rhs);
}

map_entry *map_iter_deref(map_iter mi) {
  return (map_entry*) linked_list_iter_deref(mi->lli);
}

void map_iter_destroy(map_iter mi) {
  linked_list_iter_destroy(mi->lli);
  free(mi);
}
