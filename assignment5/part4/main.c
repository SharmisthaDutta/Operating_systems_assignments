#include "scheduler.h"
#include <stdio.h>

void print_nth_prime(void * pn) {
  int n = *(int *) pn;
  int c = 1, i = 1;
  while(c <= n) {
    ++i;
    int j, isprime = 1;
    for(j = 2; j < i; ++j) {
      if(i % j == 0) {
        isprime = 0;
        break;
      }
    }
    if(isprime) {
      ++c;
    }
    yield();
  }
  printf("%dth prime: %d\n", n, i);
 
}


int main(void) {
  printf("starting from main\n");
  scheduler_begin();
  printf("returning here for 1st time\n");
  int n1 = 2, n2 = 10, n3 = 3;
  thread_fork(print_nth_prime, &n1);
  printf("returning here for 2nd time\n");
  thread_fork(print_nth_prime, &n2);
  printf("returning here for 3rd time\n");
  thread_fork(print_nth_prime, &n3);

  printf("returning here for 4th time\n");
  scheduler_end();
  printf("I am in the end now\n");
}

