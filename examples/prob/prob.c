#include <stdio.h>
#include <klee/klee.h>

typedef unsigned char uchar;

int main()
{
   uchar a, b, c;

	// Make the input symbolic.
	klee_make_symbolic(&a, sizeof a, "a");
	klee_make_symbolic(&b, sizeof b, "b");
	klee_make_symbolic(&c, sizeof c, "c");

   // a: 0-9
   klee_assume(a >= 0);
   klee_assume(a <= 9);

   // b: 0-4
   klee_assume(b >= 0);
   klee_assume(b <= 4);

   // c: 0-2
   klee_assume(c >= 0);
   klee_assume(c <= 2);

   if (a + b >= 8) {
      if (c >= 1) {
         printf("path 1\n");
      } else {
         printf("path 2\n");
      }
   } else {
      if (c >= 2) {
         printf("path 3\n");
      } else {
         printf("path 4\n");
      }
   }

  return 0;
}
