#ifndef SERVER_INTERNAL_CONNECTION_H
#define SERVER_INTERNAL_CONNECTION_H

#include <pthread.h>

#include "../connection.h"
#include "../server.h"
#include "../../protocol/protocol.h"

#define CONN_UNUSED (-1)
#define CONN_CONNECTED (0)
#define CONN_ALIVE (1)
#define CONN_DEAD (2)

#define CLIENT_RECV_THREAD_INACTIVE 0
#define CLIENT_RECV_THREAD_ACTIVE 1

struct connection {
  server *srv;

  int sock;
  char* name;

  pthread_t recv_thread;
  int recv_thread_status;
  message *recv_thread_msg;
  int recv_thread_res;
  int triggered;
};

connection *_connection_init(server *srv, int sock);
void _connection_destroy(connection *conn);
void _connection_process(connection *conn);
message *_connection_collect(connection *conn);
void _connection_run_recv(connection *conn);
void _connection_close(connection *conn);

void _connection_trigger(connection *conn, fd_set *fds);

#endif
