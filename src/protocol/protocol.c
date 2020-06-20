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
#define CHECKPOINT if(!_send_can_continue(res)) return res;

  int res = MSG_SEND_OK;

  res = send(sock, &msg->type, sizeof(char), 0);
  CHECKPOINT;

  switch(msg->type) {
  case MSG_TYPE_TEXT: {
    res = send(sock, &msg->content.text.len, sizeof(int), 0);
    CHECKPOINT;
    res = send(sock, msg->content.text.buffer, msg->content.text.len, 0);
    CHECKPOINT;
  } break;
  case MSG_TYPE_HEARTBEAT:
    break;
  default: {
    res = MSG_SEND_ERR;
  }
  }

  return res;
#undef CHECKPOINT
}

message *message_create(int type, message_content *content) {
  message *msg = (message*) malloc(sizeof(message));
  if(msg == NULL) return NULL;

  msg->type = type;
  memcpy(&msg->content, content, sizeof(message_content));
  switch(msg->type) {
  case MSG_TYPE_TEXT: {
    msg->content.text.buffer = (char*) malloc(msg->content.text.len);
    memcpy(&msg->content.text.buffer, content->text.buffer, msg->content.text.len);
  } break;
  case MSG_TYPE_HEARTBEAT:
  default: {}
  }

  return msg;
}

void message_destroy(message *msg) {
  switch(msg->type) {
  case MSG_TYPE_TEXT: {
    free(msg->content.text.buffer);
  } break;
  case MSG_TYPE_HEARTBEAT:
  default:
    break;
  }
  free(msg);
}

char *msg_to_cstr(message *msg) {
  if(msg->type != MSG_TYPE_TEXT) return NULL;

  int len = msg->content.text.len + 1;

  char *str = (char*) malloc(len);
  memcpy(str, msg->content.text.buffer, len - 1);
  str[len - 1] = '\0';
  return str;
}

message *cstr_to_msg(const char *txt) {
  message_content content;
  content.text.buffer = (char*) txt;
  content.text.len = strlen(txt);
  return message_create(MSG_TYPE_TEXT, &content);
}
