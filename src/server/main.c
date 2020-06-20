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
  printf("Connection established on socket %d\n", connection_get_socket(conn));
  return 0;
}

int my_on_conn_disconnected(server *srv, connection *conn) {
  UNUSED(srv);
  my_log(2);
  printf("Connection #%d disconnected\n", connection_get_socket(conn));
  return 0;
}

int my_on_conn_recv(server *srv, connection *conn, message *msg) {
  UNUSED(srv);
  my_log(2);
  printf("Connection #%d received a message.", connection_get_socket(conn));

  if(message_get_type(msg) == MSG_TYPE_TEXT) {
    char *buf = msg_to_cstr(msg, 256);
    printf("  %s\n", buf);
    free(buf);
  }

  return 0;
}

int my_on_conn_authenticated(server *srv, connection *conn) {
  UNUSED(srv);
  my_log(2);
  printf("%d will now be called %s\n", connection_get_socket(conn), connection_get_name(conn));
  return 0;
}

int my_on_broadcast(server *srv, connection *conn, const char *msg) {
  UNUSED(srv);
  my_log(2);
  printf("<%d:%s>: %s", connection_get_socket(conn), connection_get_name(conn), msg);
  return 0;
}

int main() {
  server_settings settings;
  server_settings_init(&settings);
  settings.max_conns = 5;
  settings.name_size = 32;
  settings.max_msg_size = 1024;

  settings.cbs.on_server_start = my_on_server_start;

  settings.cbs.on_before_create_conn = my_on_before_create_conn;
  settings.cbs.on_after_create_conn = my_on_after_create_conn;
  settings.cbs.on_conn_disconnected = my_on_conn_disconnected;
  settings.cbs.on_conn_authenticated = my_on_conn_authenticated;

  settings.cbs.on_conn_recv = my_on_conn_recv;

  settings.cbs.on_protocol_error = my_on_protocol_error;


  server* srv = server_create(&settings);
  int res;
  res = server_listen(srv, 12345);
  assert(res == 0);
  res = server_start(srv);
  assert(res == 0);

  server_wait(srv);
  server_destroy(srv);

  return 0;
}
