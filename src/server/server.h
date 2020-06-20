// server.h
// Date: 2020-06-21
// Author: 18TI018 Haralanov Valeri

// This file defines the API specification for
// the server I implemented for socket communication.

#ifndef SERVER_H
#define SERVER_H

#include "util.h"
#include "../collections/map.h"
#include "../collections/queue.h"
#include "../protocol/protocol.h"

#include "connection.h"

// The server struct shall remain opaque
// in order to preserve binary compatibility.
typedef struct server server;

// This server is implemented almost completely
// asynchronously, using callbacks. Each event
// causes a function to be called with parameters relevant to it.
// This structure is used to store the various callbacks.
typedef struct {
  int (*on_server_start)(server *srv);

  int (*on_before_create_conn)(server *srv);
  int (*on_after_create_conn)(server *srv, connection *conn);
  int (*on_conn_disconnected)(server *srv, connection *conn);

  int (*on_conn_has_data)(server *srv, connection *conn);
  int (*on_conn_recv)(server *srv, connection *conn, message *msg);

  int (*on_conn_authenticated)(server *srv, connection *conn);
  int (*on_broadcast)(server *srv, connection *conn, const char *msg);

  int (*on_conn_name_too_large)(server *srv, connection *conn, message *msg);
  int (*on_protocol_error)(server *srv, connection *conn, message *msg, int res);

  void (*on_internal_error)(server *srv, const char *msg);
} server_callbacks;

// This structure represents the customization options for the server.
typedef struct {
  int port;
  int max_msg_size;
  int max_name_size;
  server_callbacks cbs;
} server_settings;

// This function populates the server_settings structure
// with meaningful values, ensuring correct operation of
// the later procedures.
void server_settings_init(server_settings* settings);

// This function constructs a server with the given settings.
// At the end of the call, the server is not bound to or listening on
// on any socket. It simply stores the information it needs in memory.
server* server_create(const server_settings* settings);

// This function destroys a server object and frees any allocated memory
// associated with it. HOWEVER, it assumes that all connection-related
// cleanup has finished and frees only memory used by the server when
// it is not running. In order to free up connections as well, see server_stop
void server_destroy(server *srv);

// This function starts the server.
// At the end of the call, the server is bound to and listening
// on a socket, and is running in the background thread. This
// function returns immediately after starting the other thread
// and should therefore be generally followed by server_wait
int server_start(server *srv);

// This function sends a stop signal the server.
// It returns immediately and should therefore be
// followed by server_wait and finally server_destroy
// in order to completely free all used resources.
void server_stop(server *srv);

// This function waits for the server to stop by itself.
// This is useful in the main thread since it simply blocks
// until server_stop has been called and all connection-specific
// resources have been released.
void server_wait(server *srv);

// This function broadcasts a message to all connections in the server.
void server_broadcast(server *srv, connection *conn, const char *msg);

#endif
