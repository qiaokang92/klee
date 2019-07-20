#include <stdio.h>
#include <klee/klee.h>

#define HASH_SIZE 1024
#define NUM_PKT 1
#define ALARM_THRES 2

#define CACHE_EMPTY 0
#define CACHE_USED  0

typedef unsigned char uchar;
typedef unsigned int uint;

// Now we don't deal with real hash functions
// The hash function just returns incremental integer % HASH_SIZE.
#define REAL_HASH 0

int hash(int x)
{
   static int ret = 0;
#if REAL_HASH
   return x % HASH_SIZE;
#else
   return (ret ++) % HASH_SIZE;
#endif
}

int main()
{
   int i = 0;
   // input packets
   uchar pkt_id[NUM_PKT];
   uchar pkt_re_flag[NUM_PKT]; // 0: normal; 1: retransmission

   // per connection state: 0: normal; 1: retransmitting
   int ht_key[HASH_SIZE] = {0};
   int ht_val[HASH_SIZE] = {0};
   int ht_used[HASH_SIZE] = {0};

   // Make the input symbolic.
   klee_make_symbolic(pkt_id, sizeof pkt_id, "pkt_id");
   klee_make_symbolic(pkt_re_flag, sizeof pkt_re_flag, "pkt_re_flag");
   for (i = 0; i < NUM_PKT; i ++) {
      klee_assume(pkt_re_flag[i] <= 1);
      klee_assume(pkt_re_flag[i] >= 0);
   }

   // iteration for each packet
   for (i = 0; i < NUM_PKT; i ++) {
     printf("prcessing packet %d\n", i);

     // get hash table index
     int index = hash(pkt_id[i]);

     // empty entry
     if (ht_used[index] == 0) {
        ht_key[index] = pkt_id[i];
        ht_used[index] = 1;
     }
     // cache collision
     else if (ht_used[index] == 1 && ht_key[index] != pkt_id[i]) {
        // evict the old entry
        ht_key[index] = pkt_id[i];
     }
     // cache hit: do nothing

     // update connection state
     if (pkt_re_flag[i] == 1) {
         ht_val[index] = 1;
     } else {
         ht_val[index] = 0;
     }

     int sum = ht_val[0] + ht_val[1] + ht_val[2] + ht_val[3];

     if (sum >= ALARM_THRES) {
        printf("%s\n", "alarming!");
        ht_val[0] = ht_val[1] = ht_val[2] = ht_val[3] = 0;
     } else {
        printf("%s\n", "normal forwarding");
     }
  }

  return 0;
}
