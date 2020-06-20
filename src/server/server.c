#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "internal/server.h"

int min(int a, int b) {
  return a > b ? b : a;
}

void server_settings_init(server_settings* settings) {
  settings->max_conns = 0;
  settings->max_msg_size = 0;

  server_callbacks cbs;
  cbs.on_server_start = NULL;

  cbs.on_before_create_conn = NULL;
  cbs.on_after_create_conn = NULL;
  cbs.on_conn_disconnected = NULL;

  cbs.on_conn_has_data = NULL;
  cbs.on_conn_recv = NULL;
  cbs.on_conn_authenticated = NULL;

  cbs.on_broadcast = NULL;

  cbs.on_protocol_error = NULL;

  memcpy(&settings->cbs, &cbs, sizeof(server_callbacks));

  settings->cbs.on_conn_overflow = NULL;
  settings->cbs.on_client_init = NULL;
  settings->cbs.on_client_msg = NULL;
  settings->cbs.on_internal_error = NULL;
}

server* server_create(const server_settings* settings) {
  server thesrv;
  server *srv = &thesrv;

  srv->client_conns = map_init();
  srv->conn_sock = socket(PF_INET, SOCK_STREAM, 0);
  if(srv->conn_sock == -1) {
    SRV_CALLBACK(on_internal_error, "socket() failed");

    return NULL;
  }

  int temp = 1;
  int res = setsockopt(srv->conn_sock, SOL_SOCKET, SO_REUSEADDR, (void *) &temp, sizeof(temp));
  if(res == -1) {
    SRV_CALLBACK(on_internal_error, "setsockopt() failed");
    return NULL;
  }

  srv = (server*) malloc(sizeof(server));
  memcpy(srv, &thesrv, sizeof(server));
  memcpy(&srv->settings, settings, sizeof(server_settings));
  return srv;
}

int server_listen(server* srv, int port) {
  struct sockaddr_in srv_addr;
  memset((void *) &srv_addr, 0, sizeof(srv_addr));
  srv_addr.sin_family = PF_INET;
  srv_addr.sin_port = htons(port);
  srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int res = bind(srv->conn_sock, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
  if(res != 0) {
    SRV_CALLBACK(on_internal_error, "bind() failed");
    return -1;
  }

  res = listen(srv->conn_sock, 5 /* TODO */);
  if(res != 0) {
    SRV_CALLBACK(on_internal_error, "listen() failed");
    return res;
  }

  return 0;
}

int server_start(server *srv) {
  int res = pthread_create(&srv->srv_thread, NULL, _t_server_run, (void*) srv);
  if(res != 0) {
    SRV_CALLBACK(on_internal_error, "server_start(): pthread_create() failed");
    return res;
  }

  return 0;
}

void server_broadcast(server *srv, connection *conn, const char *msg) {
  SRV_CALLBACK(on_broadcast, conn, msg);

  int bufsize = srv->settings.name_size + srv->settings.max_msg_size + 10;
  message_content content;
  content.text.buffer = (char*) malloc(bufsize);
  int real_size = snprintf(content.text.buffer, bufsize, "[TIMESTAMP] <%d:%s>: %s", conn->sock, conn->name, msg);
  content.text.len = min(real_size + 1, bufsize);

  message *themsg = message_create(MSG_TYPE_TEXT, &content);

  MAP_FOREACH(mi, srv->client_conns) {
    send_message(map_iter_deref(mi)->k, themsg);
  } MAP_FOREACH_END(mi);
}

void server_wait(server *srv) {
  pthread_join(srv->srv_thread, NULL);
}

void server_destroy(server *srv) {
  srv->running = 0;

  MAP_FOREACH(mi, srv->client_conns) {
    _connection_destroy((connection*) map_iter_deref(mi)->v);
  } MAP_FOREACH_END(mi);
}
