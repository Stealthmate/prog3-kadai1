#ifndef UTIL_H
#define UTIL_H

#include "master.h"

#include <pthread.h>

void cleanup_unlock_mutex(void *arg);
void cleanup_close_socket(void *arg);

#endif
