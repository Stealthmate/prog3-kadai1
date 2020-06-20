#include "linked_list.h"

#include <stdlib.h>
struct A {
  int value;
  int value1;
  double value2;
};

typedef struct _linked_list_cell {
  struct _linked_list_cell *prev;
  void *item;
  struct _linked_list_cell *next;
} _linked_list_cell;

struct linked_list {
  _linked_list_cell head;
  _linked_list_cell tail;
};

struct _linked_list_iter {
  linked_list *ll;
  _linked_list_cell *cell;
};

_linked_list_cell *_linked_list_insert_cell(_linked_list_cell *prev, void *item, _linked_list_cell *next) {
  _linked_list_cell *llc = (_linked_list_cell*) malloc(sizeof(_linked_list_cell));
  if(llc == NULL) return NULL;

  llc->prev = prev;
  llc->item = item;
  llc->next = next;

  prev->next = llc;
  next->prev = llc;

  return llc;
}

void* _linked_list_delete_cell(_linked_list_cell *llc) {
  void *item = llc->item;
  llc->prev->next = llc->next;
  llc->next->prev = llc->prev;
  free(llc);
  return item;
}

linked_list* linked_list_init() {
  linked_list *ll = (linked_list*) malloc(sizeof(linked_list));

  ll->head.prev = NULL;
  ll->head.item = NULL;
  ll->head.next = &ll->tail;

  ll->tail.prev = &ll->head;
  ll->tail.item = NULL;
  ll->tail.next = NULL;

  return ll;
}

void linked_list_destroy(linked_list *ll) {
  free(ll);
}

int linked_list_is_empty(linked_list *ll) {
  return ll->head.next == &ll->tail;
}

int linked_list_prepend(linked_list *ll, void *item) {
  _linked_list_cell *llc = _linked_list_insert_cell(&ll->head, item, ll->head.next);
  return llc == NULL ? COLLECTION_NOMEM : COLLECTION_OK;
}

int linked_list_append(linked_list *ll, void *item) {
  _linked_list_cell *llc = _linked_list_insert_cell(ll->tail.prev, item, &ll->tail);
  return llc == NULL ? COLLECTION_NOMEM : COLLECTION_OK;
}

int linked_list_pop_head(linked_list *ll, void **item) {
  if(ll->head.next == NULL) return -1;

  if(linked_list_is_empty(ll)) {
    *item = NULL;
    return COLLECTION_EMPTY;
  }

  *item = _linked_list_delete_cell(ll->head.next);

  return COLLECTION_OK;
}

int linked_list_pop_tail(linked_list *ll, void **item) {
  if(ll->tail.prev == NULL) return -1;

  if(linked_list_is_empty(ll)) {
    *item = NULL;
    return COLLECTION_EMPTY;
  }

  *item = _linked_list_delete_cell(ll->tail.prev);

  return COLLECTION_OK;
}


linked_list_iter linked_list_begin(linked_list *ll) {
  linked_list_iter lli = (linked_list_iter) malloc(sizeof(struct _linked_list_iter));
  lli->cell = ll->head.next;
  lli->ll = ll;
  return lli;
}

linked_list_iter linked_list_end(linked_list *ll) {
  linked_list_iter lli = (linked_list_iter) malloc(sizeof(struct _linked_list_iter));
  lli->cell = &ll->tail;
  lli->ll = ll;
  return lli;
}

linked_list_iter linked_list_find(linked_list *ll, void *item, eq_f f) {
  linked_list_iter lli = linked_list_begin(ll);
  linked_list_iter end = linked_list_end(ll);
  while(!linked_list_iter_eq(lli, end) && (!f(lli->cell->item, item)))
    linked_list_iter_incr(lli);

  linked_list_iter_destroy(end);
  return lli;
}

int linked_list_insert_after(linked_list_iter lli, void *item) {
  _linked_list_cell *cell = (_linked_list_cell*) malloc(sizeof(_linked_list_cell));
  if(cell == NULL) return -1;
  cell->prev = lli->cell;
  cell->next = lli->cell->next;
  cell->item = item;
  lli->cell->next->prev = cell;
  lli->cell->next = cell;

  lli->cell = cell;
  return 0;
}

int linked_list_insert_before(linked_list_iter lli, void *item) {
  _linked_list_cell *cell = (_linked_list_cell*) malloc(sizeof(_linked_list_cell));
  if(cell == NULL) return COLLECTION_ILLEGAL_ARG;
  cell->prev = lli->cell->prev;
  cell->next = lli->cell;
  cell->item = item;
  lli->cell->prev->next = cell;
  lli->cell->prev = cell;

  lli->cell = cell;
  return 0;
}

int linked_list_pop(linked_list_iter lli, void **item) {
  _linked_list_cell *cell = lli->cell;
  if(cell == &lli->ll->tail) {
    *item = NULL;
    return COLLECTION_ILLEGAL_ARG;
  }

  lli->cell = cell->next;
  cell->prev->next = cell->next;
  cell->next->prev = cell->prev;
  *item = cell->item;
  free(cell);

  return COLLECTION_OK;
}

int linked_list_iter_incr(linked_list_iter lli) {
  if(lli->cell == &lli->ll->tail) return COLLECTION_STOP_ITER;

  lli->cell = lli->cell->next;
  return COLLECTION_OK;
}

int linked_list_iter_decr(linked_list_iter lli) {
  if(lli->cell == lli->ll->head.next) return COLLECTION_STOP_ITER;

  lli->cell = lli->cell->prev;
  return COLLECTION_OK;
}

int linked_list_iter_eq(linked_list_iter lhs, linked_list_iter rhs) {
  return lhs->cell == rhs->cell;
}

void linked_list_iter_cp_assign(linked_list_iter lhs, linked_list_iter rhs) {
  lhs->cell = rhs->cell;
}

void linked_list_iter_mv_assign(linked_list_iter lhs, linked_list_iter rhs) {
  linked_list_iter_cp_assign(lhs, rhs);
  linked_list_iter_destroy(rhs);
}

void *linked_list_iter_deref(linked_list_iter lli) {
  return lli->cell->item;
}

void linked_list_iter_destroy(linked_list_iter lli) {
  free(lli);
}
