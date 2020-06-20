#include "protocol.h"

#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#include "internal/protocol.h"

int recv_message(int sock, int max_size, message **msg) {

  char msg_type;
  int res = _recv_type(sock, &msg_type);
  if(!_recv_can_continue(res)) return res;

  message_content content;
  res = MSG_RECV_OK;

  switch(msg_type) {
  case MSG_TYPE_HEARTBEAT:
    break;
  case MSG_TYPE_TEXT: {
    res = _recv_content_text(sock, max_size, &content.text);
  } break;
  case MSG_TYPE_SERVER_HANDSHAKE: {
    res = _recv_content_server_handshake(sock, &content.server_handshake);
  } break;
  case MSG_TYPE_DISCONNECT:
    break;
  default: {
    res = MSG_RECV_ERR;
  }
  }

  if(res == MSG_RECV_OK || res == MSG_RECV_TOO_LONG) {
    *msg = message_create(msg_type, &content);
  } else {
    *msg = NULL;
  }
  return res;
}


message_content *message_get_content(message *msg) {
  return &msg->content;
}

int message_get_type(message *msg) {
  return msg->type;
}

int send_message(int sock, message* msg) {
  int res = MSG_SEND_OK;

  send(sock, &msg->type, sizeof(char), 0);

  switch(msg->type) {
  case MSG_TYPE_TEXT: {
    send(sock, &msg->content.text.len, sizeof(int), 0);
    send(sock, msg->content.text.buffer, msg->content.text.len, 0);
  } break;
  case MSG_TYPE_SERVER_HANDSHAKE: {
    send(sock, &msg->content.server_handshake.max_content_size, sizeof(int), 0);
    send(sock, &msg->content.server_handshake.max_name_size, sizeof(int), 0);
  }
  case MSG_TYPE_HEARTBEAT:
    break;
  default: {
    res = MSG_SEND_ERR;
  }
  }

  return res;
}

message *message_create(int type, message_content *content) {
  message *msg = (message*) malloc(sizeof(message));
  if(msg == NULL) return NULL;

  msg->type = type;
  memcpy(&msg->content, content, sizeof(message_content));

  return msg;
}

void message_destroy(message *msg) {
  switch(msg->type) {
  case MSG_TYPE_TEXT: {
    free(msg->content.text.buffer);
  } break;
  case MSG_TYPE_SERVER_HANDSHAKE:
  case MSG_TYPE_HEARTBEAT:
  case MSG_TYPE_DISCONNECT:
  default:
    break;
  }

  free(msg);
}

char *msg_to_cstr(message *msg, int max) {
  if(msg->type != MSG_TYPE_TEXT) return NULL;

  int len = msg->content.text.len + 1;
  int size = max > len ? len : max;

  char *str = (char*) malloc(size);
  memcpy(str, msg->content.text.buffer, size - 1);
  str[size - 1] = '\0';
  return str;
}
