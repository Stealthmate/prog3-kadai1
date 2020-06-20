#include "util.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void cleanup_unlock_mutex(void *arg) {
  pthread_mutex_unlock((pthread_mutex_t*) arg);
}

int select_socks(int *socks, int n, int us, fd_set *fds) {
  FD_ZERO(fds);

  for(int i=0;i<n;i++) {
    FD_SET(socks[i], fds);
  }

  struct timeval tv;
  tv.tv_sec = us / 1000;
  tv.tv_usec = us % 1000;

  return select(FD_SETSIZE, fds, NULL, NULL, us > 0 ? &tv : NULL);
}

int sock_accept(int conn_sock) {
  struct sockaddr_in client;
  memset((void *) &client, 0, sizeof(client));
  socklen_t fromlen = sizeof(client);

  return accept(conn_sock, (struct sockaddr *) &client, &fromlen);
}
