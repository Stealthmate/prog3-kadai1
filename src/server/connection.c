#include "connection.h"
#include "internal/connection.h"

int connection_get_socket(connection *conn) {
  return conn->sock;
}

char *connection_get_name(connection *conn) {
  return conn->name;
}
