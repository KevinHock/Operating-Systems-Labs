#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "trie.h"

extern unsigned int tid_array[200000];

typedef int bool;
#define TRUE 1
#define FALSE 0

#define PTN printThreadNumber();
#define UPTN unlockForPTN();
#define LPTN lockForPTN();

//Comment out here       //  if you don't want to see the atomic print
#define ATOMIC_PRINT(...) LPTN PTN printf("Line #%d ",__LINE__); printf(__VA_ARGS__); UPTN

#define LINES 1
#ifdef LINES
#define LINESP PTN printf("%d\n",__LINE__);// UPTN
#else
#define LINESP
#endif

int searchNoLock(const char *string, size_t strlen, int32_t *ip4_address);

struct trie_node {
  struct trie_node *next;	        /* Parent list */
  unsigned int strlen;		        /* Length of the key */
  int32_t ip4_address; 		        /* 4 octets */
  struct trie_node *children; 	  /* Sorted list of children */
  char key[64]; 		              /* Up to 64 chars */
};

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
static struct trie_node * root = NULL;

static pthread_mutex_t mutexForPTN = PTHREAD_MUTEX_INITIALIZER;

/*static*/ int numWaiting=1;

void printThreadNumber(){
  //Loop through tid_array and when it matches print what number thread it is.
  int i=0;
  for(;i<200000;i++){
    if(tid_array[i]==(unsigned int)pthread_self()){
      printf("Thread #%d ",(i+1));
    }
  }
}

void lockForPTN(){
  int err=pthread_mutex_lock(&mutexForPTN);
  assert(!err);  
}

void unlockForPTN(){
  pthread_mutex_unlock(&mutexForPTN);
}

struct trie_node *new_leaf(const char *string, size_t strlen, int32_t ip4_address) {
  struct trie_node *new_node = malloc(sizeof(struct trie_node));
  if (!new_node) {
    ATOMIC_PRINT("WARNING: Node memory allocation failed.  Results may be bogus.\n");
    return NULL;
  }
  assert(strlen < 64);
  assert(strlen > 0);
  new_node->next = NULL;
  new_node->strlen = strlen;
  strncpy(new_node->key, string, strlen);
  new_node->key[strlen] = '\0';
  new_node->ip4_address = ip4_address;
  new_node->children = NULL;

  return new_node;
}

int compare_keys (const char *string1, int len1, const char *string2, int len2, int *pKeylen) {
    int keylen, offset1, offset2;
    keylen = len1 < len2 ? len1 : len2;
    offset1 = len1 - keylen;
    offset2 = len2 - keylen;
    assert (keylen > 0);
    if (pKeylen)
      *pKeylen = keylen;
    return strncmp(&string1[offset1], &string2[offset2], keylen);
}

void init(int numthreads) {
  printf("You have %d threads.\n",numthreads);
  root = NULL;
}

/* Recursive helper function.
 * Returns a pointer to the node if found.
 * Stores an optional pointer to the 
 * parent, or what should be the parent if not found.
 */
struct trie_node *_search (struct trie_node *node, const char *string, size_t strlen) {
	 
  int keylen, cmp;

  // First things first, check if we are NULL 
  if (node == NULL) return NULL;

  assert(node->strlen < 64);

  // See if this key is a substring of the string passed in
  cmp = compare_keys(node->key, node->strlen, string, strlen, &keylen);
  if (cmp == 0) {
    // Yes, either quit, or recur on the children

    // If this key is longer than our search string, the key isn't here
    if (node->strlen > keylen) {
      return NULL;
    } else if (strlen > keylen) {
      // Recur on children list
      return _search(node->children, string, strlen - keylen);
    } else {
      assert (strlen == keylen);
      return node;
    }

  } else if (cmp < 0) {
    // No, look right (the node's key is "less" than the search key)
    return _search(node->next, string, strlen);
  } else {
    // Quit early
    return 0;
  }
}

int search(const char *string, size_t strlen, int32_t *ip4_address) {
  struct trie_node *found;
  bool foundNode;
  int err;

  //ATOMIC_PRINT("In search trying to get lock. String is  %s.\n",string);
  err=pthread_mutex_lock(&mutex);
  assert(!err); 
  ATOMIC_PRINT("GOT LOCK IN SEARCH\n");
  
  // Skip strings of length 0
  if (strlen == 0){
    pthread_mutex_unlock(&mutex);
    ATOMIC_PRINT("Unlocked returning 0 since strlen is zero\n");
    return 0;
  }
  
  //ATOMIC_PRINT("Searching for \"%s\" of length %u\n", string, strlen);
  found = _search(root, string, strlen);
 
  if (found && ip4_address){
    *ip4_address = found->ip4_address;
  }
  foundNode=(found != NULL);  

  pthread_mutex_unlock(&mutex);      
  //ATOMIC_PRINT("Done with search\n"); 
  return foundNode;
}

/* Recursive helper function */
int _insert (const char *string, size_t strlen, int32_t ip4_address, struct trie_node *node, struct trie_node *parent, struct trie_node *left) {
  int cmp, keylen;

  // First things first, check if we are NULL 
  assert (node != NULL);
  assert (node->strlen < 64);

  // Take the minimum of the two lengths
  cmp = compare_keys (node->key, node->strlen, string, strlen, &keylen);
  if (cmp == 0) {
    // Yes, either quit, or recur on the children

    // If this key is longer than our search string, we need to insert
    // "above" this node
    if (node->strlen > keylen) {
      struct trie_node *new_node;

      assert(keylen == strlen);
      assert((!parent) || parent->children == node);

      new_node = new_leaf (string, strlen, ip4_address);
      node->strlen -= keylen;
      new_node->children = node;

      assert ((!parent) || (!left));

      if (parent) {
        parent->children = new_node;
      } else if (left) {
        left->next = new_node;
      } else if ((!parent) || (!left)) {
        root = new_node;
      }
      return 1;

    } else if (strlen > keylen) {
      
      if (node->children == NULL) {
        // Insert leaf here
        struct trie_node *new_node = new_leaf (string, strlen - keylen, ip4_address);
        node->children = new_node;
        return 1;
      } else {
        // Recur on children list, store "parent" (loosely defined)
        return _insert(string, strlen - keylen, ip4_address,
           node->children, node, NULL);
      }
    } else {
      assert (strlen == keylen);
      if (node->ip4_address == 0) {
        node->ip4_address = ip4_address;
        return 1;
      } else {
        return 0;
      }
    }

  } else {
    /* Is there any common substring? */
    int i, cmp2, keylen2, overlap = 0;
    for (i = 1; i < keylen; i++) {
      cmp2 = compare_keys (&node->key[i], node->strlen - i, 
			   &string[i], strlen - i, &keylen2);
      assert (keylen2 > 0);
      if (cmp2 == 0) {
        overlap = 1;
        break;
      }
    }

    if (overlap) {
      // Insert a common parent, recur
      struct trie_node *new_node = new_leaf (&string[i], strlen - i, 0);
      int diff = node->strlen - i;
      assert ((node->strlen - diff) > 0);
      node->strlen -= diff;
      new_node->children = node;
      assert ((!parent) || (!left));

      if (node == root) {
        new_node->next = node->next;
        node->next = NULL;
        root = new_node;
      } else if (parent) {
        assert(parent->children == node);
        new_node->next = NULL;
        parent->children = new_node;
      } else if (left) {
        new_node->next = node->next;
        node->next = NULL;
        left->next = new_node;
      } else if ((!parent) && (!left)) {
        root = new_node;
      }

      return _insert(string, i, ip4_address,
		     node, new_node, NULL);
    } else if (cmp < 0) {
      if (node->next == NULL) {
        // Insert here
        struct trie_node *new_node = new_leaf (string, strlen, ip4_address);
        node->next = new_node;
        return 1;
      } else {
        // No, recur right (the node's key is "greater" than  the search key)
        return _insert(string, strlen, ip4_address, node->next, NULL, node);
      }
    } else {
        // Insert here
        struct trie_node *new_node = new_leaf (string, strlen, ip4_address);
        new_node->next = node;
        if (node == root)
          root = new_node;
        else if (parent && parent->children == node)
          parent->children = new_node;
    }
    return 1;
  }
}

int insert (const char *string, size_t strlen, int32_t ip4_address) {
  int everWaited=0,repeat,result,rvOWait,err;
  
  //ATOMIC_PRINT("In insert trying to get lock. String is  %s.\n",string);
  err=pthread_mutex_lock(&mutex);
  assert(!err);
  ATOMIC_PRINT("GOT THE LOCK IN INSERT\n");
  
  // Skip strings of length 0
  if (strlen == 0){
    pthread_mutex_unlock(&mutex);
    ATOMIC_PRINT("Unlocked returning 0 since strlen is zero\n");
    return 0;
  }

  // Edge case: root is null
  if (root == NULL) {
    root = new_leaf (string, strlen, ip4_address);
    pthread_mutex_unlock(&mutex);
    return 1;
  }


  int32_t *pass=&ip4_address; 
  if(searchNoLock(string,strlen,pass)){
    repeat=1; 
    //ATOMIC_PRINT("Repeat is equal to one.\n");
  }else{
    //ATOMIC_PRINT("No repeat.\n");
    repeat=0;
  }
  
  //While squatting on AND string in trie then wait
  while(repeat && allow_squatting){
    everWaited=1;

    ATOMIC_PRINT("%d-th time waiting\tString is %s\n", numWaiting++, string);
    
    rvOWait = pthread_cond_wait(&cond,&mutex);
    
    if(rvOWait!=0){
      ATOMIC_PRINT("GODOGOD pthread_cond_wait ERROR  GODOGOD pthread_cond_wait ERROR  GODOGOD pthread_cond_wait ERROR  \n\n\n");
    }else{
      ATOMIC_PRINT("Return value of wait is %d.\n",rvOWait);
    }
    if(searchNoLock(string,strlen,pass)){
      repeat=1;
    }else{
      repeat=0;
    }
  }
  
  //If we ever waited
  if(everWaited){
    ATOMIC_PRINT("Just got woken up and now trying to insert string \"%s\" of length %u\n\n\n\n\n\n\n", string, strlen);
    
    // Skip strings of length 0
    if (strlen == 0){
      pthread_mutex_unlock(&mutex);
      return 0;
    }
  
    // Edge case: root is null
    if (root == NULL) {
      root = new_leaf (string, strlen, ip4_address);
      pthread_mutex_unlock(&mutex);
      return 1;
    }
  }
  
  result = _insert (string, strlen, ip4_address, root, NULL, NULL);
  
  if(result){
    //ATOMIC_PRINT("Insert successful\n");
  }else{
    //ATOMIC_PRINT("Insert not successful. Result is %d String is %s of length %u \n",result,string,strlen);
  }
  
  //Unlock mutex
  pthread_mutex_unlock(&mutex);
  ATOMIC_PRINT("Done with insert\n");
  return result;
}

int searchNoLock(const char *string, size_t strlen, int32_t *ip4_address){
  struct trie_node *found;
  bool foundNode;
  
  // Skip strings of length 0
  if (strlen == 0){
    return 0;
  }

  found = _search(root, string, strlen);
  
  if (found && ip4_address){
    *ip4_address = found->ip4_address;
  }
  
  foundNode=(found != NULL);     
  return foundNode;
}


/* Recursive helper function.
 * Returns a pointer to the node if found.
 * Stores an optional pointer to the 
 * parent, or what should be the parent if not found.
 * 
 */
struct trie_node *_delete (struct trie_node *node, const char *string, size_t strlen) {
    int keylen, cmp;

  // First things first, check if we are NULL 
  if (node == NULL) return NULL;

  assert(node->strlen < 64);

  // See if this key is a substring of the string passed in
  cmp = compare_keys (node->key, node->strlen, string, strlen, &keylen);
  if (cmp == 0) {
    // Yes, either quit, or recur on the children

    // If this key is longer than our search string, the key isn't here
    if (node->strlen > keylen) {
      return NULL;
    } else if (strlen > keylen) {
      struct trie_node *found =  _delete(node->children, string, strlen - keylen);
      if (found) {
	/* If the node doesn't have children, delete it.
	 * Otherwise, keep it around to find the kids */
	if (found->children == NULL && found->ip4_address == 0) {
	  assert(node->children == found);
	  node->children = found->next;
	  free(found);
	}
	
	/* Delete the root node if we empty the tree */
	if (node == root && node->children == NULL && node->ip4_address == 0) {
	  root = node->next;
	  free(node);
	}
	
	return node; /* Recursively delete needless interior nodes */
      } else 
	return NULL;
    } else {
      assert (strlen == keylen);

      /* We found it! Clear the ip4 address and return. */
      if (node->ip4_address) {
	node->ip4_address = 0;

	/* Delete the root node if we empty the tree */
	if (node == root && node->children == NULL && node->ip4_address == 0) {
	  root = node->next;
	  free(node);
	  return (struct trie_node *) 0x100100; /* XXX: Don't use this pointer for anything except 
						 * comparison with NULL, since the memory is freed.
						 * Return a "poison" pointer that will probably 
						 * segfault if used.
						 */
	}
	return node;
      } else {
	/* Just an interior node with no value */
	return NULL;
      }
    }

  } else if (cmp < 0) {
    // No, look right (the node's key is "less" than  the search key)
    struct trie_node *found = _delete(node->next, string, strlen);
    if (found) {
      /* If the node doesn't have children, delete it.
       * Otherwise, keep it around to find the kids */
      if (found->children == NULL && found->ip4_address == 0) {
	assert(node->next == found);
	node->next = found->next;
	free(found);
      }       

      return node; /* Recursively delete needless interior nodes */
    }
    return NULL;
  } else {
    // Quit early
    return NULL;
  }
/*
  int keylen, offset1, offset2, cmp;

  // First things first, check if we are NULL 
  if (node == NULL) return NULL;

  assert(node->strlen < 64);

  // Take the minimum of the two lengths
  keylen = node->strlen < strlen ? node->strlen : strlen;
  offset1 = node->strlen - keylen;
  offset2 = strlen - keylen;

  // See if this key is a substring of the string passed in
  cmp = strncmp(&node->key[offset1], &string[offset2], keylen);
  if (cmp == 0) {
    // Yes, either quit, or recur on the children

    // If this key is longer than our search string, the key isn't here
    if (node->strlen > keylen) {
      return NULL;
    } else if (strlen > keylen) {
      struct trie_node *found =  _delete(node->children, string, strlen - keylen);
      if (found) {
	// If the node doesn't have children, delete it.
  // Otherwise, keep it around to find the kids 
	if (found->children == NULL && found->ip4_address == 0) {
	  if (node->next == found)
	    node->next = found->next;
	  else
	    node->children = found->next;
	  free(found);
	}
	return node; // Recursively delete needless interior nodes 
      } else 
	return NULL;
    } else {
      assert (strlen == keylen);

      // We found it! Clear the ip4 address and return.
      if (node->ip4_address) {
	node->ip4_address = 0;
	return node;
      } else {
	// Just an interior node with no value
	return NULL;
      }
    }

  } else if (cmp < 0) {
    // No, look right (the node's key is "less" than  the search key)
    struct trie_node *found = _delete(node->next, string, strlen);
    if (found) {
      // If the node doesn't have children, delete it.
       // Otherwise, keep it around to find the kids 
      if (found->children == NULL && found->ip4_address) {
        if (node->next == found)
          node->next = found->next;
        else
          node->children = found->next;
	free(found);
      }
      return node; // Recursively delete needless interior nodes 
    }
    return NULL;
  } else {
    // Quit early
    return NULL;
  }
*/
}

int delete(const char *string, size_t strlen) {
  int result,rvOBroadcast,err;
  
  //ATOMIC_PRINT("In delete attempting to get lock. String is %s of length %u\n", string, strlen);
  err=pthread_mutex_lock(&mutex);
  assert(!err);
  ATOMIC_PRINT("GOT LOCK IN DELETE\n"); 
  
  // Skip strings of length 0
  if (strlen == 0){
    ATOMIC_PRINT("Strlen is zero. Going to unlock\n");
    pthread_mutex_unlock(&mutex);
    ATOMIC_PRINT("Unlocked returning 0 since strlen is zero\n");
    return 0;
  }
  
  //ATOMIC_PRINT("Attempting to delete \"%s\" of length %u\n",string, strlen);
  result = (NULL != _delete(root, string, strlen));
  pthread_mutex_unlock(&mutex);
  
  if(result){
    // Since we deleted something then we can notify all threads waiting 
    rvOBroadcast = pthread_cond_broadcast(&cond);
    if(rvOBroadcast!=0){
      ATOMIC_PRINT("GODOGOD pthread_cond_broadcast ERROR\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); 
    }
    ATOMIC_PRINT("Deleted %s  successful BROADCASTING.\n",string);
  }else{
    ATOMIC_PRINT("Delete unsuccessful.\n");
  }

  return result;
}



void _print (struct trie_node *node) {
  printf ("Node at %p.  Key %.*s, IP %d.  Next %p, Children %p\n", 
	  node, node->strlen, node->key, node->ip4_address, node->next, node->children);
  
  if (node->children)
    _print(node->children);
  if (node->next)
    _print(node->next);
}

void print(){
  int err;
  //ATOMIC_PRINT("Attempting to acquire lock in print()\n");
  err=pthread_mutex_lock(&mutex);
  assert(!err);
  if(root!=NULL){
    _print(root);
  }else{
    ATOMIC_PRINT("Root is null in print\n");
  }
  pthread_mutex_unlock(&mutex);
  //ATOMIC_PRINT("Done with print()\n");
}

