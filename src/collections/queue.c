#include "queue.h"

#include <pthread.h>
#include <stdlib.h>

#include "linked_list.h"

static void _cleanup_unlock_mutex(void *arg) {
  pthread_mutex_unlock((pthread_mutex_t*) arg);
}

struct queue {
  linked_list *ll;
  pthread_mutex_t mutex;
};

queue *queue_init() {
  queue *q = (queue*) malloc(sizeof(queue));
  q->ll = linked_list_init();
  pthread_mutex_init(&q->mutex, NULL);

  return q;
}

void queue_destroy(queue* q) {
  pthread_mutex_destroy(&q->mutex);
  linked_list_destroy(q->ll);
  free(q);
}

#define Q_LOCK {                                \
  pthread_mutex_lock(&q->mutex);\
  pthread_cleanup_push(_cleanup_unlock_mutex, &q->mutex);

#define Q_UNLOCK\
  pthread_cleanup_pop(1);\
  }

int queue_is_empty(queue *q) {
  int res;

  Q_LOCK;
  res = linked_list_is_empty(q->ll);
  Q_UNLOCK;

  return res;
}

int queue_put(queue *q, void *item) {
  int res = 0;

  Q_LOCK;
  res = linked_list_append(q->ll, item);
  Q_UNLOCK;

  return res;
}

int queue_pop(queue *q, void **item) {
  int res = 0;

  Q_LOCK;
  res = linked_list_pop_head(q->ll, item);
  Q_UNLOCK;

  return res;
}
