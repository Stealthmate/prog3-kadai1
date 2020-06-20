#ifndef COLLECTIONS_COLLECTIONS_H
#define COLLECTIONS_COLLECTIONS_H

#define COLLECTION_OK 0
#define COLLECTION_EMPTY -1
#define COLLECTION_FULL -2
#define COLLECTION_NOMEM -3
#define COLLECTION_ILLEGAL_ARG -4
#define COLLECTION_STOP_ITER -5

typedef int (*eq_f)(void *lhs, void *rhs);

#endif
