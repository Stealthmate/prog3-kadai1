#include "interactive_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../collections/linked_list.c"

struct interactive_log {
  interactive_log_settings settings;
  linked_list *lines;
  char *sep;
  int current;
};

int _log_height(interactive_log *il) {
  return il->settings.height - 2;
}

void interactive_log_settings_init(interactive_log_settings *settings) {
  settings->width = 32;
  settings->height = 10;
}

interactive_log *interactive_log_create(interactive_log_settings *settings) {
  interactive_log *il = (interactive_log*) malloc(sizeof(interactive_log));
  memcpy(&il->settings, settings, sizeof(interactive_log_settings));

  il->lines = linked_list_init();
  for(int i=0;i<_log_height(il);i++) {
    char* l = (char*) malloc(il->settings.width + 2);
    memset(l, '\0', il->settings.width);
    l[strlen(l)] = '\n';
    l[il->settings.width + 1] = '\0';
    linked_list_prepend(il->lines, l);
  }
  il->current = 0;
  il->sep = (char*) malloc(il->settings.width + 2);
  memset(il->sep, '-', il->settings.width + 1);
  il->sep[il->settings.width] = '\n';
  il->sep[il->settings.width + 1] = '\0';

  return il;
}

void interactive_log_post(interactive_log *il, const char *msg) {

  int w = strlen(msg);
  int width = il->settings.width;
  int h = (w / width) + (w % width != 0);

  for(int i=0;i<h;i++) {
    char *l = (char*) malloc(width + 2);
    memset(l, '\0', width + 2);
    strncpy(l, &msg[i * width], width);
    l[strlen(l)] = '\n';
    linked_list_append(il->lines, l);

    linked_list_pop_head(il->lines, (void**) &l);
    free(l);
  }
}

char** interactive_log_render(interactive_log *il) {
  char **lines = (char**) malloc(sizeof(char*) * il->settings.height);
  linked_list_iter lli;
  linked_list_iter end = linked_list_end(il->lines);
  int i = 0;


  for(lli = linked_list_begin(il->lines); !linked_list_iter_eq(lli, end); linked_list_iter_incr(lli)) {
    lines[i] = linked_list_iter_deref(lli);
    i += 1;
  }

  lines[il->settings.height - 2] = il->sep;
  lines[il->settings.height - 1] = il->settings.prompt;

  linked_list_iter_destroy(lli);
  linked_list_iter_destroy(end);
  return lines;
}
