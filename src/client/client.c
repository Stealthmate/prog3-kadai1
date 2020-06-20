#include "client.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "../protocol/protocol.h"
#include "../collections/queue.h"

#define CLNT_CALLBACK(cb, ...)          \
  if(clnt->settings.cb) {                      \
    clnt->settings.cb(clnt, __VA_ARGS__);      \
  }

struct client {
  client_settings settings;
  int sock;

  int running;

  pthread_rwlock_t lock;

  pthread_t recv_thread;
  pthread_t send_thread;

  queue *q_recv;
  queue *q_send;
};

int _get_running(client *clnt) {
  pthread_rwlock_rdlock(&clnt->lock);
  int r = clnt->running;
  pthread_rwlock_unlock(&clnt->lock);
  return r;
}

void _set_running(client *clnt, int val) {
  pthread_rwlock_rdlock(&clnt->lock);
  clnt->running = val;
  pthread_rwlock_unlock(&clnt->lock);
}

void *_t_client_recv(void *arg) {
  client *clnt = (client*) arg;

  int local_running = 1;

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  while(local_running) {
    local_running = _get_running(clnt);

    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(clnt->sock, &fd);

    int res = select(FD_SETSIZE, &fd, NULL, NULL, &tv);
    if(res == -1) {
      break;
    }

    message *msg;
    recv_message(clnt->sock, clnt->settings.max_content_size, &msg);

    int type = message_get_type(msg);
    message_content *content = message_get_content(msg);
    switch(type) {
    case MSG_TYPE_TEXT: {
      char *buffer = (char*) malloc(content->text.len + 1);
      memcpy(buffer, content->text.buffer, content->text.len);
      buffer[content->text.len] = '\0';
      queue_put(clnt->q_recv, buffer);
    } break;

    case MSG_TYPE_HEARTBEAT: //TODO
    default: break;
    }
  }

  _set_running(clnt, 0);

  return NULL;
}

void *_t_client_send(void *arg) {
  client *clnt = (client*) arg;

  int local_running = 1;
  message *msg;
  while(local_running) {
    local_running = _get_running(clnt);

    int res = queue_pop(clnt->q_send, (void**) &msg);
    if(res == COLLECTION_EMPTY) {
      usleep(100);
      continue;
    }

    puts("SENDING");

    send_message(clnt->sock, msg);
    message_destroy(msg);
  }

  _set_running(clnt, 0);

  return NULL;
}

void client_settings_init(client_settings *settings) {
  settings->max_content_size = 0;
  settings->on_internal_error = NULL;
  settings->on_msg_recv = NULL;
}

client *client_create(client_settings *settings) {
  client *clnt = (client*) malloc(sizeof(client));
  memcpy(&clnt->settings, settings, sizeof(client_settings));
  clnt->sock = -1;

  clnt->q_recv = queue_init();
  clnt->q_send = queue_init();

  return clnt;
}

void client_start(client *clnt, const char *host, int port) {

  clnt->sock = socket(PF_INET, SOCK_STREAM, 0);
  if (clnt->sock == -1) {
    CLNT_CALLBACK(on_internal_error, "socket() failed");
  }

  struct sockaddr_in server;
  memset((void *) &server, 0, sizeof(server));
  server.sin_family = PF_INET;
  server.sin_port = htons(port);

  struct hostent* hent = gethostbyname(host);

  memcpy(&server.sin_addr, hent->h_addr_list[0], hent->h_length);

  if (connect(clnt->sock, (struct sockaddr *) &server, sizeof(server)) == -1) {
    CLNT_CALLBACK(on_internal_error, "connect() failed");
    return;
  }

  _set_running(clnt, 1);

  pthread_create(&clnt->recv_thread, NULL, _t_client_recv, clnt);
  pthread_create(&clnt->send_thread, NULL, _t_client_send, clnt);
}

int client_is_running(client *clnt) {
  return _get_running(clnt);
}

int client_fetch(client *clnt, char **msg) {
  message *themsg;

  int res = queue_pop(clnt->q_recv, (void**) &themsg);
  if(res == COLLECTION_EMPTY) return 1;

  *msg = msg_to_cstr(themsg);

  message_destroy(themsg);

  return 0;
}

void client_send(client *clnt, const char *msg) {
  queue_put(clnt->q_send, cstr_to_msg(msg));
}

void client_stop(client *clnt) {
  _set_running(clnt, 0);
  close(clnt->sock);
}

void client_destroy(client *clnt) {
  //TODO
}
