#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include "trie.h"
//
static int threadNumber=1;

int allow_squatting = 0;
int simulation_length = 30; // default to 30 seconds
volatile int finished = 0;

// Uncomment this line for debug printing
//#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) 
#endif

static void *client(void *arg){
  tid_array[threadNumber-1] = (unsigned int)pthread_self();
  printf("Thread #%d in squatter stress!\n",threadNumber++);
  
  struct random_data rd;
  char rand_state[256];
  int32_t salt = time(0);

  rd.state = (int32_t*)rand_state;

  // Initialize the prng.  For testing, it may be helpful to reduce noise by 
  // temporarily setting this to a fixed value.
  initstate_r(salt, rand_state, sizeof(rand_state), &rd);

  while (!finished) {

    /* Pick a random operation, string, and ip */
    int32_t code;
    int rv = random_r(&rd, &code);
    int length = (code >> 2) & (64-1);
    char buf[64];
    int j;
    int32_t ip4_addr;
    
    if (rv) {
      printf("Failed to get random number - %d\n", rv);
      return NULL;
    }

    DEBUG_PRINT("Length is %d\n", length);
    memset(buf, 0, 64);

    /* Generate a random string in lowercase */
    for (j = 0; j < length; j+= 6) {
      int i;
      int32_t chars;

      rv = random_r(&rd, &chars);
      if (rv) {
        printf("Failed to get random number - %d\n", rv);
        return NULL;
      }

      for (i = 0; i < 6 && (i+j) < length; i++) {
        char val = ( (chars >> (5 * i)) & 31);
        if (val > 25)
          val = 25;
        buf[j+i] = 'a' + val;
      }
    }

    DEBUG_PRINT ("Random string is %s\n", buf);

    switch (code % 3) {
    case 0: // Search
      DEBUG_PRINT ("Search\n");
      search (buf, length, NULL);
      break;
    case 1: // insert
      DEBUG_PRINT ("insert\n");
      rv = random_r(&rd, &ip4_addr);
      if (rv) {
        printf("Failed to get random number - %d\n", rv);
        return NULL;
      }
      insert (buf, length, ip4_addr);
      break;
    case 2: // delete
      DEBUG_PRINT ("delete\n");
      delete (buf, length);
      break;
    default:
      assert(0);
    }
  }

  return NULL;
}

static void *squatter_stress(void *arg){
  tid_array[threadNumber-1] = (unsigned int)pthread_self();
  printf("Thread #%d in squatter stress!\n",threadNumber++);
  
  /* Just loop over 4 names, trying to create them
   * and then free them.  If all threads
   * try to create the same name, all but one should block
   * until a given name is available again.
   */
  int32_t ip = random();

  while (!finished) {
    insert ("abc", 3, ip);
    insert ("abe", 3, ip);
    insert ("bce", 3, ip);
    insert ("bcc", 3, ip);
    //insert ("zzz", 3, ip);
    delete ("abc", 3);
    delete ("abe", 3);
    delete ("bce", 3);
    delete ("bcc", 3);
    printf("\n\n\n\n\n\nEnd of squatter loop, now printing tree in ");
    printThreadNumber();
    printf(":\n");
    print();
  }
  
  printf("Time is up.\n");
  return NULL;
}

#define die(msg) do {		\
  printf("\n\n");           \
  print();					\
  printf("\n\n");           \
  printf(msg);				\
  exit(1);					\
  } while (0)

int self_tests() {
  int rv;
  int32_t ip = 0;

  DEBUG_PRINT("\nRUNNING SELF TESTS:\n");
  DEBUG_PRINT("-----------------------------\n");
  
  DEBUG_PRINT("01] Adding root node: ['.com']....\n");
  rv = insert (".com", 4, 100);
  if (!rv) die ("Could not insert root node.\n\n");
  DEBUG_PRINT("Succesfully inserted root node: ['.com']....\n\n");
  
  DEBUG_PRINT("02] Inserting node: ['google.com']....\n");
  rv = insert("google.com", 10, 376006);
  if (!rv) die ("Failed to insert key bb\n");
  DEBUG_PRINT("Succesfully inserted key: ['google.com']....\n\n");

  DEBUG_PRINT("03] Inserting node: ['bing.com']....\n");
  rv = insert("bing.com", 8, 6218);
  if (!rv) die ("Failed to insert key bb\n");
  DEBUG_PRINT("Succesfully inserted key: ['bing.com']....\n\n");

  DEBUG_PRINT("04] Printing....\n");
  print();
  DEBUG_PRINT("Successfully Printed Trie....\n\n");
  
  DEBUG_PRINT("05] Searching for: ['google.com']....\n");
  rv = search("google.com", 10, &ip);
  printf("Search Result: [Return Value: %d] [IP: %d]\n", rv, ip);
  if (!rv) die ("Failed to find key ['google.com']\n");
  if (ip != 376006) die ("Bad IP returned for ['google.com']\n");
  DEBUG_PRINT("Successfully found key: ['google.com']....\n\n");
  
  DEBUG_PRINT("06] Searching for: ['youtube.com']....\n");
  rv = search("youtube.com", 11, NULL);
  printf("Search Result: [Return Value: %d] [IP: %d]\n", rv, ip);
  if (rv) die ("Found nonexistent key: ['youtube.com']\n");
  DEBUG_PRINT("Failed to find key: ['youtube.com']....\n\n");

  ip = 0;

  DEBUG_PRINT("07] Searching for: ['bing.com']....\n");
  rv = search("bing.com", 8, &ip);
  printf("Search Result: [Return Value: %d] [IP: %d]\n", rv, ip);
  if (!rv) die ("Failed to find key ['bing.com']\n\n");
  if (ip != 6218) die ("Found bad IP for key ['bing.com']\n");
  DEBUG_PRINT("Successfully found key: ['bing.com']....\n\n");

  ip = 0;
  DEBUG_PRINT("08] Deleting node: ['yahoo.com']....\n");
  rv = delete("yahoo.com", 9);
  if (rv) die ("Deleted nonexistent key ['yahoo.com']\n");
  DEBUG_PRINT("Failed to delete key: ['yahoo.com']....\n\n");

  DEBUG_PRINT("09] Deleting node: ['bing.com']....\n");
  rv = delete("bing.com", 8);
  if (!rv) die ("Failed to delete key: ['bing.com']....\n\n");
  DEBUG_PRINT("Successfully deleted key: ['bing.com']....\n\n");

  DEBUG_PRINT("10] Searching for: ['bing.com']....\n");
  rv = search("bing.com", 8, &ip);
  printf("Search Result: [Return Value: %d] [IP: %d]\n", rv, ip);
  if (rv) die ("Found nonexistent key ab\n");
  DEBUG_PRINT("Failed to find key: ['bing.com']....\n\n");

  ip = 0;
  DEBUG_PRINT("11] Deleting node: ['google.com']....\n");
  rv = delete("google.com", 10);
  if (!rv) die ("Failed to delete key: ['google.com']....\n\n");
  DEBUG_PRINT("Successfully deleted key: ['google.com']....\n\n");

  printf("Self test succeeded!\n");
  return 0;
}

void help() {
  printf ("DNS Simulator.  Usage: ./dns-[variant] [options]\n\n");
  printf ("Options:\n");
  printf ("\t-c numclients - Use numclients threads.\n");
  printf ("\t-h - Print this help.\n");
  printf ("\t-l length - Run clients for length seconds.\n");
  printf ("\t-q  - Allow a client to block (squat) if a requested name is taken.\n");
  printf ("\t-t  - Stress test name squatting.\n");
  printf ("\n\n");
}

int main(int argc, char ** argv) {
  int numthreads = 1; // default to 1
  int c, i, rv;
  pthread_t *tinfo;
  int stress_squatting = 0;


  // Read options from command line:
  //   # clients from command line, as well as seed file
  //   Simulation length
  //   Block if a name is already taken ("Squat")
  //   Stress test "squatting"
  while ((c = getopt (argc, argv, "c:hl:qt")) != -1) {
    switch (c) {
    case 'c':
      numthreads = atoi(optarg);
      break;
    case 'h':
      help();
      return 0;
    case 'l':
      simulation_length = atoi(optarg);
      break;
    case 'q':
      allow_squatting = 1;
      break;
    case 't':
      stress_squatting = 1;
      break;
    default:
      printf ("Unknown option\n");
      help();
      return 1;
    }
  }

  
  // Create initial data structure, populate with initial entries
  // Note: Each variant of the tree has a different init function, statically compiled in
  init(numthreads);
  srandom(time(0));

  // Run the self-tests if we are in debug mode 
  #ifdef DEBUG
      self_tests();
  #endif

  // Launch client threads
  tinfo = calloc(numthreads, sizeof(pthread_t));
  for (i = 0; i < numthreads; i++) {

    if (stress_squatting) {
      rv = pthread_create(&tinfo[i], NULL,
			  &squatter_stress, &i);
    } else {
      rv = pthread_create(&tinfo[i], NULL,
			  &client, NULL);
    }
    if (rv != 0) {
      printf ("Thread creation failed %d\n", rv);
      return rv;
    }
  }

  // After the simulation is done, shut it down
  sleep (simulation_length);
  finished = 1;
  //if(allow_squatting)
  //char* const* qq="lol"; 
  //char* const* wow[]={qq,NULL};
 // char ** args = {"lol",NULL};
  //execlp("echo","lol",NULL);
  printf("Times up, have a great day.\n");
  // Wait for all clients to exit.  If we are allowing blocking,
  // cancel the threads, since they may hang forever
  if (allow_squatting) {
      for (i = 0; i < numthreads; i++) {
        int rv = pthread_cancel(tinfo[i]);
        if (rv != 0)
          printf ("Uh oh.  pthread_cancel failed %d\n", rv);
      }
  }

  if(!allow_squatting){
  for (i = 0; i < numthreads; i++) {
    int rv = pthread_join(tinfo[i], NULL);
    if (rv != 0)
      printf ("Uh oh.  pthread_join failed %d\n", rv);
  }
}
  #ifdef DEBUG  
    /* Print the final tree for fun */
    print();
  #endif
  
  return 0;
}

