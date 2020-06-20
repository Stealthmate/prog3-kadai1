#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../collections/linked_list.h"

int main() {
  printf("Init list: ");
  linked_list* ll = linked_list_init();
  puts("OK!");

  printf("Prepend 5 items: ");
  for(int i=0;i<5;i++) {
    printf("%d ", i);

    int *j = (int*) malloc(sizeof(int));
    *j = i;
    linked_list_prepend(ll, j);
  }
  puts("OK!");

  printf("Append 5 items: ");
  for(int i=5;i<10;i++) {
    printf("%d ", i);

    int *j = (int*) malloc(sizeof(int));
    *j = i;
    linked_list_append(ll, j);
  }
  puts("OK!");

  linked_list_iter begin = linked_list_begin(ll);
  linked_list_iter end = linked_list_end(ll);
  linked_list_iter lli = linked_list_begin(ll);
  printf("Iterate forward: ");
  while(!linked_list_iter_eq(lli, end)) {
    printf("%d ", *(int*) linked_list_iter_deref(lli));
    linked_list_iter_incr(lli);
  }
  puts("OK!");

  printf("Iterate backward: ");
  linked_list_iter_cp_assign(lli, end);
  while(linked_list_iter_decr(lli) == 0) {
    printf("%d ", *(int*) linked_list_iter_deref(lli));
  }
  puts("OK!");

  printf("Try going backward beyond begin(): ");
  linked_list_iter_cp_assign(lli, begin);
  assert(linked_list_iter_decr(lli) != 0);
  puts("OK!");

  printf("Try going forward beyond end(): ");
  linked_list_iter_cp_assign(lli, end);
  assert(linked_list_iter_incr(lli) != 0);
  puts("OK!");

  printf("Pop 2nd element: ");
  linked_list_iter_cp_assign(lli, begin);
  linked_list_iter_incr(lli);
  int* j = (int*) linked_list_pop(lli);
  printf("%d ", *j);
  free(j);
  puts("OK!");

  linked_list_iter_destroy(begin);
  linked_list_iter_destroy(end);

  printf("Insert after 2nd element: ");
  j = (int*) malloc(sizeof(int));
  *j = 10;
  linked_list_insert_after(lli, j);
  printf("%d ", *j);
  puts("OK!");

  printf("Insert before 3rd element: ");
  j = (int*) malloc(sizeof(int));
  *j = 20;
  linked_list_insert_before(lli, j);
  printf("%d ", *j);
  puts("OK!");

  linked_list_iter_destroy(lli);

  printf("Pop 3 elements from head: ");
  for(int i=0;i<3;i++) {
    assert(linked_list_pop_head(ll, (void**) &j) == 0);
    printf("%d ", *j);
    free(j);
  }
  puts("OK!");

  printf("Pop rest from tail: ");
  while(!linked_list_is_empty(ll)) {
    assert(linked_list_pop_tail(ll,(void**)  &j) == 0);
    printf("%d ", *j);
    free(j);
  }
  puts("OK!");

  printf("Destroy list: ");
  linked_list_destroy(ll);
  puts("OK!");
}
