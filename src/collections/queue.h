#ifndef COLLECTIONS_QUEUE_H
#define COLLECTIONS_QUEUE_H

#include "collections.h"

typedef struct queue queue;

queue *queue_init();
void queue_destroy(queue *q);

int queue_is_empty(queue *q);
int queue_put(queue *q, void *item);
int queue_pop(queue *q, void **item);

#endif
