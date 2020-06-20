#ifndef SERVER_INTERNAL_SERVER_H
#define SERVER_INTERNAL_SERVER_H

#include "../server.h"

#include "connection.h"

#define SRV_CALLBACK(cb, ...)                                \
  if(srv->settings.cbs.cb) {                                 \
    srv->settings.cbs.cb(srv, __VA_ARGS__);              \
  }

#define SRV_CALLBACKV(cb)                                    \
  if(srv->settings.cbs.cb) {                                 \
    srv->settings.cbs.cb(srv);                               \
  }

#define SRV_CALLBACK_R(cb, r, ...)                           \
  if(srv->settings.cbs.cb) {                                 \
    r = srv->settings.cbs.cb(srv, __VA_ARGS__);              \
  }

#define SRV_CALLBACK_(cb, r)                        \
  if(srv->settings.cbs.cb) {                        \
    r = srv->settings.cbs.cb(srv);                  \
  }

#define CLIENT_RECV_THREAD_INACTIVE 0
#define CLIENT_RECV_THREAD_ACTIVE 1

struct server {
  server_settings settings;
  int conn_sock;
  map *client_conns;

  int running;
  pthread_t srv_thread;

  queue *job_q;
};

typedef struct {
  server *srv;
} t_server_run_args;

void *_t_server_run(void *arg);
void _server_proc_jobs(server *srv);
void _server_proc_conn_sock(server *srv);
void _server_proc_conn_triggers(server *srv);
void _server_proc_client_socks(server *srv);

void _server_accept_new_conn(server *srv);

typedef struct {
  server *srv;
  connection *conn;
} t_server_protocol_recv_args;

void *_t_server_protocol_recv(void *arg);

void _run_server_protocol_recv(server *srv, connection *conn);

int _conn_recv_has_finished(connection *conn);

#endif
