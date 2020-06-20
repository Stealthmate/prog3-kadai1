#include "protocol.h"

#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>

#define DROP_BUF_SIZE 1024

int min(int a, int b) {
  return a > b ? b : a;
}

int _recv_can_continue(int res) {
  return res != MSG_RECV_ERR
     && res != MSG_RECV_DISCONNECTED;
}

int _recv_exactly(int sock, int size, char *buffer) {
  int read = 0;
  while(read < size) {
    int s = recv(sock, buffer + read, size - read, 0);
    if(s == 0) return MSG_RECV_DISCONNECTED;
    read += s;
  }

  return MSG_RECV_OK;
}

int _drop_exactly(int sock, int size, int bufsize) {
  if(size == 0) return MSG_RECV_OK;

  int res = MSG_RECV_OK;

  int dropped = 0;
  char *buf = (char*) malloc(bufsize);

  pthread_cleanup_push(free, buf);

  while(dropped < size) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10;
    select(FD_SETSIZE, &fds, NULL, NULL, &tv);
    if(FD_ISSET(sock, &fds)) {
      int s = recv(sock, buf, min(bufsize, size - dropped), 0);
      if(s == 0) {
        res = MSG_RECV_DISCONNECTED;
        break;
      }
      dropped += s;
    }
  }

  pthread_cleanup_pop(1);
  return res;
}

int _recv_content_text(int sock, int max_size, message_content_text *text) {
  int real_size;
  int res = _recv_exactly(sock, sizeof(int), (char*) &real_size);
  if(!_recv_can_continue(res)) return res;

  text->len = min(real_size, max_size);

  text->buffer = (char*) malloc(text->len);

  res = _recv_exactly(sock, text->len, text->buffer);
  if(!_recv_can_continue(res)) goto cleanup;
  res = _drop_exactly(sock, real_size - text->len, DROP_BUF_SIZE);
  if(!_recv_can_continue(res)) goto cleanup;

  return real_size > max_size ? MSG_RECV_TOO_LONG : MSG_RECV_OK;
 cleanup:
  free(text->buffer);;
  return res;
}

int _recv_type(int sock, char *msg_type) {
  return _recv_exactly(sock, sizeof(char), msg_type);
}

int _send_can_continue(int res) {
  return res == MSG_SEND_OK;
}
