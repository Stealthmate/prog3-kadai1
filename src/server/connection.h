#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

typedef struct connection connection;

int connection_get_socket(connection *conn);
char *connection_get_name(connection *conn);

#endif
