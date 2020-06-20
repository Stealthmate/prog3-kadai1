#ifndef COLLECTIONS_LINKED_LIST_H
#define COLLECTIONS_LINKED_LIST_H

#include "collections.h"

typedef struct linked_list linked_list;
typedef struct _linked_list_iter* linked_list_iter;


linked_list *linked_list_init();
void linked_list_destroy(linked_list *ll);

int linked_list_is_empty(linked_list *ll);
int linked_list_prepend(linked_list *ll, void *item);
int linked_list_append(linked_list *ll, void *item);
int linked_list_pop_head(linked_list *ll, void **item);
int linked_list_pop_tail(linked_list *ll, void **item);

linked_list_iter linked_list_begin(linked_list *ll);
linked_list_iter linked_list_end(linked_list *ll);
linked_list_iter linked_list_find(linked_list *ll, void *item, eq_f f);

int linked_list_insert_after(linked_list_iter lli, void *item);
int linked_list_insert_before(linked_list_iter lli, void *item);
int linked_list_pop(linked_list_iter lli, void **item);

int linked_list_iter_incr(linked_list_iter lli);
int linked_list_iter_decr(linked_list_iter lli);

int linked_list_iter_eq(linked_list_iter lhs, linked_list_iter rhs);

void linked_list_iter_cp_assign(linked_list_iter lhs, linked_list_iter rhs);
void linked_list_iter_mv_assign(linked_list_iter lhs, linked_list_iter rhs);

void *linked_list_iter_deref(linked_list_iter lli);

void linked_list_iter_destroy(linked_list_iter lli);


#endif
