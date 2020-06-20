#include "../interactive_log/interactive_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_lines(char **lines, int n) {
  printf("\033[2J");
  for(int i=0;i<n;i++) {
    printf("%s", lines[i]);
  }
}

int main() {
  interactive_log_settings ils;
  interactive_log_settings_init(&ils);
  ils.width = 20;
  ils.height = 5;
  ils.prompt = "> ";

  interactive_log *il = interactive_log_create(&ils);

  char **lines = interactive_log_render(il);
  print_lines(lines, ils.height);
  while(1) {
    char mybuf[256];
    fgets(mybuf, 255, stdin);
    mybuf[strlen(mybuf) - 1] = '\0';

    interactive_log_post(il, mybuf);
    free(lines);
    lines = interactive_log_render(il);
    print_lines(lines, ils.height);
  }
}
