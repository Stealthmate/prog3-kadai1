// protocol.h
// Date: 2020-06-21
// Author: 18TI018 Haralanov Valeri

// This file defines the API specification for the protocol I used
// to implement socket communication.

#ifndef PROTOCOL_H
#define PROTOCOL_H

// First we define the types of messages that can be sent
// A heartbeat is a message that is used to signify to each side
// that the connection is still alive
#define MSG_TYPE_HEARTBEAT 0
// A text message is simple a string of characters
#define MSG_TYPE_TEXT 1

// Now we define the structure of each message
// This protocol will only support text messages (for now)
// and so there is only one struct. However, we have
// intentionally defined message_content as a union,
// so as to allow future extensibility
typedef struct {
  char *buffer;
  int len;
} message_content_text;
typedef union {
  message_content_text text;
} message_content;

// This is the actual message structure,
// however it shall remain opaque so as to
// preserve binary compatibility between versions
typedef struct message message;

// This is the function used to recieve messages.
// This protocol is implemented on top of stream (TCP) sockets
// and so it requires a socket handle. Furthermore,
// this function performs all the necessary allocations and so it accepts
// a max_size parameter which signifies the maximum space
// to be used to store a given message.
int recv_message(int sock, int max_size, message **msg);
// Here we have the return codes for recv_message.
// Non-negative values signify whole or partial success.

// The message was received successfully.
#define MSG_RECV_OK 0
// The message was received, however it was longer than the specified max_size and was therefore truncated
#define MSG_RECV_TOO_LONG 1
// An error occurred while receiving data
#define MSG_RECV_ERR -1
// The connection was closed while receiving data
#define MSG_RECV_DISCONNECTED -2

// This is the function used to send messages.
int send_message(int sock, message *msg);
// The message was sent successfully
#define MSG_SEND_OK 0
// An error occurred while sending data.
// No guarantees concerning the validity of sent data
// are provided. The message may have been sent fully or partially
// or no data may have been written to the OS buffers.
#define MSG_SEND_ERR -1

// This function constructs a message given a message type and content.
// The content struct is copied deeply - that is,
// any buffers allocated are duplicated inside the message
// and can be freed after calling message_create
message *message_create(int msg_type, message_content *content);

// This function destroys a given message.
// Any buffers allocated as part of the content are
// freed and therefore must not be freed before or after
// calling message_destroy.
void message_destroy(message *msg);

// These functions simply retrieve the respective information
// about a message.
int message_get_type(message *msg);
message_content *message_get_content(message *msg);

// This is a utility function.
// If called on a message of type MSG_TYPE_TEXT,
// it copies the text inside the message to a
// new buffer, allocated so that it contains
// exactly the text part and a null-terminator.
char* msg_to_cstr(message *msg);

// This is another utility function.
// It performs the opposite of msg_to_cstr - that is,
// it takes a null-terminated string and constructs a
// text message based on it.
message *cstr_to_msg(const char *txt);

#endif
