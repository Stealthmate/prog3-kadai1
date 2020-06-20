#define _GNU_SOURCE
#include "connection.h"

#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"

message *_connection_collect(connection *conn) {
  server *srv = conn->srv;

  message *msg = conn->recv_thread_msg;
  conn->recv_thread_msg = NULL;
  conn->recv_thread_status = CLIENT_RECV_THREAD_INACTIVE;

  if(conn->recv_thread_res == MSG_RECV_DISCONNECTED) {
    SRV_CALLBACK(on_conn_disconnected, conn);
    _connection_close(conn);
    return NULL;
  }
  if(conn->recv_thread_res == MSG_RECV_ERR) {
    SRV_CALLBACK(on_protocol_error, conn, msg, conn->recv_thread_res);
    return NULL;
  }

  return msg;
}

connection *_connection_init(server *srv, int sock) {
  connection *conn = (connection*) malloc(sizeof(connection));
  conn->srv = srv;
  conn->sock = sock;
  conn->name = NULL;
  conn->recv_thread_status = CLIENT_RECV_THREAD_INACTIVE;
  conn->recv_thread_res = -1;
  conn->triggered = 0;
  return conn;
}

void _connection_destroy(connection *conn) {
  if(conn->name != NULL) free(conn->name);
  free(conn);
}

void _connection_process(connection *conn) {
  server *srv = conn->srv;
  conn->triggered = 0;

  if(conn->recv_thread_status == CLIENT_RECV_THREAD_INACTIVE) {
    _connection_run_recv(conn);
    return;
  }

  message *msg = _connection_collect(conn);
  if(msg == NULL) return;

  SRV_CALLBACK(on_conn_recv, conn, msg);

  int msg_type = message_get_type(msg);

  switch(msg_type) {
  case MSG_TYPE_TEXT: {
    if(message_get_content(msg)->text.len >= srv->settings.max_name_size) {
      SRV_CALLBACK(on_conn_name_too_large, conn, msg);
    } else {
      char *msg_txt = msg_to_cstr(msg);
      if(conn->name == NULL) {
        conn->name = msg_txt;
        SRV_CALLBACK(on_conn_authenticated, conn);
      } else {
        server_broadcast(srv, conn, msg_txt);
      }
    }
  } break;
  case MSG_TYPE_HEARTBEAT: {
    // TODO
  } break;
  }

  message_destroy(msg);
}

void _connection_close(connection *conn) {
  if(conn->recv_thread_status == CLIENT_RECV_THREAD_ACTIVE) {
    pthread_cancel(conn->recv_thread);
    pthread_join(conn->recv_thread, NULL);
  }

  close(conn->sock);
  if(conn->name != NULL) {
    free(conn->name);
  }
}

#include <stdio.h>

void *_t_connection_recv(void *arg) {
  connection *conn = (connection*) arg;
  server *srv = conn->srv;

  SRV_CALLBACK(on_conn_has_data, conn);
  conn->recv_thread_msg = NULL;
  conn->recv_thread_res = recv_message(conn->sock, conn->srv->settings.max_msg_size, &conn->recv_thread_msg);

  pthread_exit(NULL);
}

void _connection_run_recv(connection *conn) {
  pthread_create(&conn->recv_thread, NULL, _t_connection_recv, (void*) conn);
  conn->recv_thread_status = CLIENT_RECV_THREAD_ACTIVE;
}

void _connection_trigger(connection *conn, fd_set *fds) {
  if(conn->recv_thread_status == CLIENT_RECV_THREAD_INACTIVE) {
    conn->triggered = FD_ISSET(conn->sock, fds);
  } else {
    int res = pthread_tryjoin_np(conn->recv_thread, NULL);
    conn->triggered = res == 0;
  }
}
