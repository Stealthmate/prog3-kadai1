#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdarg.h>

#include "../util.h"

#include "connection.h"

#define DEBUG

void _printf(const char *f, ...) {
  va_list ap;
#ifdef DEBUG
  va_start( ap, f );
  vprintf( f, ap );
  va_end( ap );
#endif
}

void _puts(const char *msg) {
#ifdef DEBUG
  puts(msg);
#endif
}

void *_t_server_run(void *arg) {
  server *srv = (server*) arg;

  srv->running = 1;

  SRV_CALLBACKV(on_server_start);

  while(srv->running) {
    /* _server_proc_jobs(srv); */
    _server_proc_conn_sock(srv);
    _server_proc_conn_triggers(srv);
    _server_proc_client_socks(srv);
  }

  pthread_exit(NULL);
}

void _server_proc_jobs(server *srv) {
  //TODO
}

void _server_accept_new_conn(server *srv) {

  int sock = sock_accept(srv->conn_sock);
  if(sock == -1) {
    SRV_CALLBACK(on_internal_error, "accept() failed");
    return;
  }

  int res;
  SRV_CALLBACK_(on_before_create_conn, res);
  if(res) {
    close(sock);
    return;
  }

  connection *conn = _connection_init(srv, sock);
  if(conn == NULL) {
    SRV_CALLBACK(on_internal_error, "malloc() failed");
    return;
  }

  map_put(srv->client_conns, (key) conn->sock, conn);

  SRV_CALLBACK_R(on_after_create_conn, res, conn);
}

void _server_proc_conn_sock(server *srv) {
  fd_set fds;
  int res = select_socks(&srv->conn_sock, 1, 10, &fds);
  if(res < 0) {
    SRV_CALLBACK(on_internal_error, "select() failed");
  }
  else if (FD_ISSET(srv->conn_sock, &fds)) {
    _server_accept_new_conn(srv);
  }
}

void _server_proc_conn_triggers(server *srv) {
  fd_set fds;

  size_t n = map_size(srv->client_conns);
  int *socks = (int*) malloc(sizeof(int) * n);
  size_t i = 0;

  MAP_FOREACH(mi, srv->client_conns) {
    socks[i] = map_iter_deref(mi)->k;
    i += 1;
  } MAP_FOREACH_END(mi);

  int res = select_socks(socks, n, 1000, &fds);
  if(res < 0) {
    SRV_CALLBACK(on_internal_error, "select() failed");
    goto cleanup;
  }

  for(i = 0; i < n; i++) {
    connection *conn = (connection *) map_lookup_value(srv->client_conns, socks[i]);
    _connection_trigger(conn, &fds);
  }

 cleanup:
  free(socks);
}

void _server_proc_client_socks(server *srv) {
  MAP_FOREACH(mi, srv->client_conns) {
    connection *conn = (connection*) map_iter_deref(mi)->v;
    if(conn->triggered) _connection_process(conn);
  } MAP_FOREACH_END(mi);
}

void _server_close_client_conn(server *srv, connection *conn) {
  map_erase_key(srv->client_conns, conn->sock);
  _connection_close(conn);
  /* free(conn); */
}
