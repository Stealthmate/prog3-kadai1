#ifndef COLLECTIONS_SET_H
#define COLLECTIONS_SET_H

typedef long int (*hash_f)(void *item);
typedef struct set set;
typedef struct _set_iter* set_iter;

set* set_init(hash_f f);
int set_add(set* s, void *item);
void set_destroy(set *s);

void set_iter_destroy(set_iter si);
void set_iter_copy(set_iter lhs, set_iter rhs);
void set_iter_move(set_iter lhs, set_iter rhs);
int set_iter_equals(set_iter lhs, set_iter rhs);

set_iter set_begin(set *s);
set_iter set_end(set *s);

set_iter set_find(set *s, void *item);
int set_iter_incr(set_iter si);
void *set_iter_deref(set_iter si);
void *set_pop(set_iter si);


#endif
