#ifndef PROTOCOL_INTERNAL_PROTOCOL_H
#define PROTOCOL_INTERNAL_PROTOCOL_H

#include "../protocol.h"

#define DROP_BUF_SIZE 1024

struct message {
  char type;
  message_content content;
};

int _recv_can_continue(int res);

int _recv_type(int sock, char* msg_type);
int _recv_content_text(int sock, int max_size, message_content_text *text);
int _recv_content_server_handshake(int sock, message_content_server_handshake *sh);

int _recv_exactly(int sock, int size, char *buffer);
int _drop_exactly(int sock, int size, int bufsize);

#endif
