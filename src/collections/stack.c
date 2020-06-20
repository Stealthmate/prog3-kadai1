#include "stack.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

static void _cleanup_unlock_mutex(void *arg) {
  pthread_mutex_unlock((pthread_mutex_t*) arg);
}

struct stack {
  unsigned int maxsize;
  void **items;
  unsigned int sp;
  pthread_mutex_t mutex;
};

stack* stack_init(unsigned int maxsize) {
  stack *st = (stack*) malloc(sizeof (stack));
  st->maxsize = maxsize;
  st->items = malloc(maxsize * sizeof(void*));
  memset(st->items, 0, maxsize * sizeof(void*));
  st->sp = 0;
  int res = pthread_mutex_init(&st->mutex, NULL);
  if(res != 0) return NULL;

  return st;
}

int stack_push(stack *st, void *item) {
  int res = 0;
  pthread_mutex_lock(&st->mutex);
  pthread_cleanup_push(_cleanup_unlock_mutex, &st->mutex);

  if(st->sp >= st->maxsize) {
    res = -1;
  } else {
    st->items[st->sp] = item;
    st->sp += 1;
  }

  pthread_cleanup_pop(1);
  return res;
}

int stack_pop(stack *st, void **item) {
  int res = 0;
  pthread_mutex_lock(&st->mutex);
  pthread_cleanup_push(_cleanup_unlock_mutex, &st->mutex);

  if(st->sp == 0) {
    res = -1;
    *item = NULL;
  } else {
    *item = st->items[st->sp - 1];
    st->items[st->sp] = NULL;
    st->sp -= 1;
  }

  pthread_cleanup_pop(1);
  return res;
}

void stack_destroy(stack *st) {
  free(st->items);
  pthread_mutex_destroy(&st->mutex);
  free(st);
}
