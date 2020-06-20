#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../collections/stack.h"

stack* st;

void *foo(void *arg) {
  int s = *(int*) arg;
  (void) arg;

  for(int i=0;i<5;i++) {

    int* j = (int*) malloc(sizeof(int));
    *j = i;
    printf("[%d] Push %d\n", s, *j);
    stack_push(st, (void*) j);
    sleep(s);
  }

  puts("Done!");
  return NULL;
}


int main() {

  st = stack_init(100);
  if(st == NULL) exit(1);
  puts("Init ok");

  pthread_t t1, t2;

  int s1 = 1, s2 = 2;
  pthread_create(&t1, NULL, foo, (void*) &s1);
  pthread_create(&t2, NULL, foo, (void*) &s2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  puts("Joined threads");

  while(1) {
    int *j;
    int res = stack_pop(st, (void**) &j);
    if(res != 0) break;

    printf("Pop %d\n", *j);
    free(j);
  }


  puts("Finish");
}
