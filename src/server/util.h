#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>
#include <pthread.h>

void cleanup_unlock_mutex(void *arg);
void cleanup_close_socket(void *arg);

int select_socks(int *socks, int n, int us, fd_set *fs);

int sock_accept(int conn_sock);

#endif
