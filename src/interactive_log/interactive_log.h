#ifndef INTERACITVE_LOG_INTERACTIVE_LOG_H
#define INTERACITVE_LOG_INTERACTIVE_LOG_H

typedef struct {
  int width;
  int height;
  char *prompt;
} interactive_log_settings;

typedef struct interactive_log interactive_log;

void interactive_log_settings_init(interactive_log_settings *settings);
interactive_log *interactive_log_create(interactive_log_settings *settings);
void interactive_log_destroy(interactive_log *il);

void interactive_log_post(interactive_log *il, const char *msg);
char **interactive_log_render(interactive_log *il);

#endif
