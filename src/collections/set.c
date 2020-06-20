#include "set.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "linked_list.h"

#define INIT_SIZE 17

int max(int a, int b) {
  return a > b ? a : b;
}

typedef long int key;

struct _set_iter {
  set* s;
  int bucket;
  linked_list_iter lli;
};

struct set {
  int size;
  int load;
  linked_list **buckets;
  int *bucket_sizes;
  hash_f hash_func;
};


int _key_to_bucket(set *s, key k) {
  return k % s->size;
}

set* set_init(hash_f f) {
  set *s = (set*) malloc(sizeof(set));

  s->size = INIT_SIZE;
  s->buckets = (linked_list**) malloc(sizeof(linked_list*) * s->size);
  s->bucket_sizes = (int*) malloc(sizeof(int) * s->size);
  for(int i=0;i<s->size;i++) {
    s->buckets[i] = linked_list_init();
    s->bucket_sizes[i] = 0;
  }
  s->load = 0;
  s->hash_func = f;

  return s;
}

void set_destroy(set *s) {
  for(int i=0;i<s->size;i++) {
    linked_list_destroy(s->buckets[i]);
  }

  free(s->buckets);
  free(s->bucket_sizes);
  free(s);
}

int set_add(set *s, void *item) {
  key k = s->hash_func(item);
  int bucket = _key_to_bucket(s, k);

  int res = linked_list_append(s->buckets[bucket], item);
  if(res != 0) return res;

  s->bucket_sizes[bucket] += 1;
  s->load = max(s->bucket_sizes[bucket], s->load);

  return 0;
}

void set_iter_destroy(set_iter si) {
  linked_list_iter_destroy(si->lli);
  free(si);
}

void set_iter_copy(set_iter lhs, set_iter rhs) {
  lhs->s = rhs->s;
  lhs->bucket = rhs->bucket;
  linked_list_iter_cp_assign(lhs->lli, rhs->lli);
}

void set_iter_mv_assign(set_iter lhs, set_iter rhs) {
  set_iter_copy(lhs, rhs);
  set_iter_destroy(rhs);
}

int set_iter_equals(set_iter lhs, set_iter rhs) {
  return (lhs->s == rhs->s)
    && (lhs->bucket == rhs->bucket)
    && linked_list_iter_eq(lhs->lli, rhs->lli);
}

set_iter set_begin(set *s) {
  set_iter si = (set_iter) malloc(sizeof(struct _set_iter));
  si->s = s;
  si->bucket = 0;
  si->lli = linked_list_begin(s->buckets[0]);

  linked_list_iter end = linked_list_end(s->buckets[0]);
  if(linked_list_iter_eq(si->lli, end)) set_iter_incr(si);
  linked_list_iter_destroy(end);

  return si;
}

set_iter set_end(set *s) {
  set_iter si = (set_iter) malloc(sizeof(struct _set_iter));
  si->s = s;
  si->bucket = s->size-1;
  si->lli = linked_list_end(s->buckets[s->size - 1]);
  return si;
}

int set_iter_incr(set_iter si) {
  set *s = si->s;
  linked_list_iter end = linked_list_end(s->buckets[si->bucket]);
  int res = 0;

  linked_list_iter_incr(si->lli);

  while(linked_list_iter_eq(si->lli, end)) {
    si->bucket += 1;
    if(si->bucket == s->size) {
      si->bucket -= 1;
      res = -1;
      break;
    }
    linked_list_iter_mv_assign(si->lli, linked_list_begin(s->buckets[si->bucket]));
    linked_list_iter_mv_assign(end, linked_list_end(s->buckets[si->bucket]));
  }

  linked_list_iter_destroy(end);
  return res;
}

void *set_iter_dereference(set_iter si) {
  return linked_list_iter_deref(si->lli);
}

void *set_erase(set_iter si) {

  void *item;
  linked_list_pop(si->lli, &item);
  linked_list_iter end = linked_list_end(si->s->buckets[si->bucket]);
  if(linked_list_iter_eq(si->lli, end)) set_iter_incr(si);
  linked_list_iter_destroy(end);

  return item;
}
