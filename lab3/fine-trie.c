/* A (reverse) trie with fine-grained (per node) locks. 
 *
 * Hint: We recommend using a hand-over-hand protocol to order your locks,
 * while permitting some concurrency on different subtrees.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trie.h"

void init(int numthreads) {
  /* Your code here */
}


int insert (const char *string, size_t strlen, int32_t ip4_address) {
  /* Your code here */
  return 0;
}

int search  (const char *string, size_t strlen, int32_t *ip4_address) {
  /* Your code here */
  return 0;
}
int delete  (const char *string, size_t strlen) {
  /* Your code here */
  return 0;
}

void print() {
  /* Your code here */
}
