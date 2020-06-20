#ifndef CLIENT_H
#define CLIENT_H

typedef struct client client;

typedef struct {
  int max_content_size;

  void (*on_internal_error)(client *clnt, const char *msg);
  void (*on_msg_recv)(client *clnt, const char *msg);
} client_settings;

void client_settings_init(client_settings *settings);
client *client_create(client_settings *settings);
void client_destroy(client *clnt);

void client_start(client *clnt, const char *host, int port);
int client_is_running(client *clnt);
void client_stop(client *clnt);

int client_fetch(client *clnt, char **msg);
void client_send(client *clnt, const char *msg);

#endif
