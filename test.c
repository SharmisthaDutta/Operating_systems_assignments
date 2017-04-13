#include <stdio.h>

    void swap(long * x, long * y);

    int main(void) {
      long x = 5;
      long y = 6;
      printf("x = %ld, y = %ld\n", x, y); // x = 5, y = 6

      swap(&x, &y);
      printf("x = %ld, y = %ld\n", x, y); // x = 6, y = 5

      return 0;
    }