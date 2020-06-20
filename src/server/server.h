#ifndef SERVER_H
#define SERVER_H

#define _GNU_SOURCE

#include <stdlib.h>
#include <pthread.h>

#include "util.h"
#include "../collections/map.h"
#include "../collections/queue.h"
#include "../protocol/protocol.h"
#include "connection.h"

typedef struct server server;
typedef void (*srv_job)(server *srv, void *arg);

typedef struct {

  int (*on_server_start)(server *srv);

  int (*on_before_create_conn)(server *srv);
  int (*on_after_create_conn)(server *srv, connection *conn);
  int (*on_conn_disconnected)(server *srv, connection *conn);

  int (*on_conn_has_data)(server *srv, connection *conn);
  int (*on_conn_recv)(server *srv, connection *conn, message *msg);

  int (*on_conn_authenticated)(server *srv, connection *conn);
  int (*on_broadcast)(server *srv, connection *conn, const char *msg);

  int (*on_protocol_error)(server *srv, connection *conn, message *msg, int res);

  void (*on_conn_overflow)(server*, int sock);
  void (*on_client_name_too_long)(server*, int sock);
  void (*on_client_init)(server*, unsigned int);
  void (*on_client_disconnect)(server *srv, connection *conn);
  void (*on_client_msg)(server* srv, connection *conn, message *msg);
  void (*on_internal_error)(server*, const char*);
} server_callbacks;

typedef struct {
  int max_conns;
  size_t name_size;
  size_t max_msg_size;
  server_callbacks cbs;
} server_settings;

void server_settings_init(server_settings* settings);
server* server_create(const server_settings* settings);
int server_listen(server* srv, int port);
int server_start(server *srv);

void server_schedule_job(server *srv, srv_job msg, void *arg);
void server_broadcast(server *srv, connection *conn, const char *msg);

void server_wait(server *srv);

void server_destroy(server *srv);

#endif
