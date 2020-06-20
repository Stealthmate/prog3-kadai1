#ifndef COLLECTIONS_STACK_H
#define COLLECTIONS_STACK_H

#include "../master.h"
#include <pthread.h>

typedef struct stack stack;

stack *stack_init(unsigned int maxsize);
int stack_is_empty(stack *st);
int stack_push(stack *st, void *item);
int stack_pop(stack *st, void **item);

typedef void (*destroy_f)(void*);
void stack_destroy(stack *st);

#endif
