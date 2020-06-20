#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "../master.h"

#include <unistd.h>

#define MSG_TYPE_HEARTBEAT 0
#define MSG_TYPE_TEXT 1
#define MSG_TYPE_SERVER_HANDSHAKE 2
#define MSG_TYPE_DISCONNECT 3

#define MSG_RECV_OK 0
#define MSG_RECV_TOO_LONG 1
#define MSG_RECV_ERR -1
#define MSG_RECV_DISCONNECTED -2

#define MSG_SEND_OK 0
#define MSG_SEND_ERR -1

typedef struct {
  int max_name_size;
  int max_content_size;
} message_content_server_handshake;

typedef struct {
  char *buffer;
  int len;
} message_content_text;

typedef union {
  message_content_text text;
  message_content_server_handshake server_handshake;
} message_content;

typedef struct message message;

int recv_message(int sock, int max_size, message **msg);
int send_message(int sock, message *msg);

int message_get_type(message *msg);
message_content *message_get_content(message *msg);

message *message_create(int msg_type, message_content *content);
void message_destroy(message *msg);

char* msg_to_cstr(message *msg, int max);

#endif
