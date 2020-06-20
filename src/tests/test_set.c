#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../collections/set.h"

long int my_hash_f(void *item) {
  return *(int*) item;
}

int main() {
  printf("Init list: ");
  set *s = set_init(my_hash_f);
  puts("OK!");

  printf("Prepend 5 items: ");
  for(int i=0;i<5;i++) {
    printf("%d ", i);

    int *j = (int*) malloc(sizeof(int));
    *j = i;
    set_add(s, j);
  }
  puts("OK!");


  printf("Print all items: ");
  set_iterator si = set_begin(s);
  set_iterator end = set_end(s);
  while(!set_iterator_equals(si, end)) {
    printf("%d ", *(int*) set_iterator_dereference(si));
    set_iterator_increment(si);
  }
  set_iterator_destroy(end);
  puts("OK!");


  printf("Discard 3 items: ");
  set_iterator_move(si, set_begin(s));
  for(int i=0;i<5;i++) {
    int *j = set_erase(si);
    printf("%d ", *(int*) j);
    free(j);
  }
  puts("OK!");

  set_iterator_destroy(si);
  set_destroy(s);
}
