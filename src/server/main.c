#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "server.h"

#define UNUSED(x) (void) x;

void my_log(int lvl) {
  const char* levels[] =
    { "ERROR",
      "WARNING",
      "INFO"
    };

  printf("[%s]: ", levels[lvl]);
}

void on_internal_error(server *srv, const char *msg) {
  UNUSED(srv);
  my_log(0);
  puts(msg);
}

int my_on_protocol_error(server *srv, connection *conn, message *msg, int res) {
  UNUSED(srv);
  UNUSED(msg);

  my_log(0);
  printf("Protocol error on connection #%d - error code %d\n", connection_get_socket(conn), res);

  return 0;
}

int my_on_server_start(server *srv) {
  UNUSED(srv);
  puts("Server started");
  return 0;
}

int my_on_before_create_conn(server *srv) {
  UNUSED(srv);
  my_log(2);
  puts("New connection");
  return 0;
}

int my_on_after_create_conn(server *srv, connection *conn) {
  UNUSED(srv);
  my_log(2);
  printf("<%s::%d> has entered the chat.\n", connection_get_name(conn), connection_get_socket(conn));
  return 0;
}

int my_on_conn_disconnected(server *srv, connection *conn) {
  UNUSED(srv);
  my_log(2);
  printf("<%s::%d> has left the chat.\n", connection_get_name(conn), connection_get_socket(conn));
  return 0;
}

int my_on_conn_authenticated(server *srv, connection *conn) {
  UNUSED(srv);
  my_log(2);
  printf("<%s::%d> has authenticated.\n", connection_get_name(conn), connection_get_socket(conn));
  return 0;
}

int my_on_broadcast(server *srv, connection *conn, const char *msg) {
  UNUSED(srv);
  my_log(2);
  printf("<%d::%s>: %s\n", connection_get_socket(conn), connection_get_name(conn), msg);
  return 0;
}

int main() {
  server_settings settings;
  server_settings_init(&settings);
  settings.max_name_size = 16;
  settings.max_msg_size = 1024;
  settings.port = 12345;

  settings.cbs.on_server_start = my_on_server_start;

  settings.cbs.on_before_create_conn = my_on_before_create_conn;
  settings.cbs.on_after_create_conn = my_on_after_create_conn;
  settings.cbs.on_conn_disconnected = my_on_conn_disconnected;
  settings.cbs.on_conn_authenticated = my_on_conn_authenticated;
  settings.cbs.on_broadcast = my_on_broadcast;

  settings.cbs.on_protocol_error = my_on_protocol_error;


  server* srv = server_create(&settings);
  int res;
  res = server_start(srv);
  assert(res == 0);

  server_wait(srv);
  server_destroy(srv);

  return 0;
}
