#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "client.h"
#include "../interactive_log/interactive_log.h"

client *clnt;

void handler(int sig) {
  client_stop(clnt);
}

char STDIN_BUF[256];

int W, H;

int try_fgets(char *buf, size_t max) {
  fd_set fd;
  FD_ZERO(&fd);
  FD_SET(0, &fd);
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 1;
  int res = select(FD_SETSIZE, &fd, NULL, NULL, &tv);
  if(res == 1) {
    fgets(buf, max, stdin);
    return 0;
  }
  return -1;
}

void mv_end_of_log() {
  printf("\e[%d;0G", H - 1);
  printf("\e[0E");
}

void redraw_prompt() {
  mv_end_of_log();
  puts("\e[0J");
  char *buf = (char*) malloc(W + 1);
  memset(buf, '-', W);
  buf[W] = '\0';
  puts(buf);
  free(buf);
  printf("> ");
  fflush(stdout);
}

void restore_stdin() {
  printf("%s", STDIN_BUF);
}

void my_on_internal_error(client *clnt, const char *msg) {
  printf("[Error]: %s. Errno: %d\n", msg, errno);
}

void print_lines(char **lines) {
  mv_end_of_log();
  /* puts("\e[1J"); */
  /* printf("\e[0;0G"); */
}

void clear_prev() {
  printf("\e[1J");
}

void mv_start() {
  printf("\e0;0G");
}

void terminal_clear_log() {
  mv_end_of_log();
  clear_prev();
}

void terminal_print_log(char **lines) {
  mv_start();
  for(int i=0;i<H - 2;i++) {
    printf("%s", lines[i]);
  }
}

int main() {
  client_settings clnt_settings;

  client_settings_init(&clnt_settings);
  clnt_settings.max_content_size = 1024;
  clnt_settings.on_internal_error = my_on_internal_error;

  clnt = client_create(&clnt_settings);
  puts("Created");
  client_start(clnt, "localhost", 12345);
  puts("Started");

  while(1) {
    char buf[256];
    fgets(buf, 255, stdin);
    buf[strlen(buf) - 1] = '\0';
    client_send(clnt, buf);
  }
}

/* int main() { */
/*   client_settings clnt_settings; */

/*   client_settings_init(&clnt_settings); */
/*   clnt_settings.max_content_size = 1024; */
/*   clnt_settings.on_internal_error = my_on_internal_error; */

/*   clnt = client_create(&clnt_settings); */
/*   puts("Created"); */
/*   client_start(clnt, "localhost", 12345); */
/*   puts("Started"); */

/*   W = 30; */
/*   H = 15; */
/*   interactive_log_settings ils; */
/*   interactive_log_settings_init(&ils); */
/*   ils.width = W; */
/*   ils.height = H; */
/*   ils.prompt = "> "; */

/*   interactive_log *il = interactive_log_create(&ils); */

/*   char **lines = interactive_log_render(il); */

/*   int MAX_MSGS = 10; */
/*   int cursor = 0; */

/*   char buf[256]; */

/*   terminal_clear_log(); */
/*   redraw_prompt(); */
/*   while(client_is_running(clnt)) { */
/*     int updated = 0; */

/*     int res = try_fgets(buf, 256); */
/*     if(res == 0) { */
/*       client_send(clnt, buf); */
/*       puts("Redraw"); */
/*       terminal_clear_log(); */
/*       redraw_prompt(); */
/*       updated = 1; */
/*     } */
/*     char *msg; */
/*     res = client_fetch(clnt, &msg); */
/*     if(res == 0) { */
/*       interactive_log_post(il, msg); */
/*       lines = interactive_log_render(il); */
/*       updated = 1; */
/*     } */

/*     if(updated) { */
/*       terminal_clear_log(); */
/*       /\* terminal_print_log(lines); *\/ */
/*     } */
/*   } */

/*   client_destroy(clnt); */
/* } */
