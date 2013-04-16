#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trie.h"

void clear_locks();

/*********************************************************************\
 * struct trie_node                                                   *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   A Node describing all entries in our DNS Trie                    *
 *                                                                    *
 * Fields:                                                            *
 * ===========                                                        *
 *   next          = The list of all                                  *
 *   strlen        = The length of the entire key                     *
 *                       (including total strlen of parents)          *
 *   key           = The IP Address for the new node.                 *
 *   ip4_address   = The IP Address for the new node.                 *
 *   children      = The IP Address for the new node.                 *
 *   mutex         = The lock for the node.                           *
 \********************************************************************/
struct trie_node {
  struct trie_node *next;         /* parent list             */ 
  unsigned int strlen;            /* Length of the key       */
  int32_t ip4_address;            /* 4 octets                */
  struct trie_node *children;     /* Sorted list of children */
  char key[64];                   /* Up to 64 chars          */
  pthread_mutex_t mutex;          /* A lock for the node     */
  int num_squatters;              /* The number of squatters */
}; 


/* The root node of our trie */
static struct trie_node *root = NULL;
static pthread_mutex_t rootmutex;          /* A lock for the node     */

/* The count of our locks for debugging */
static int searchlocked = 0;
static int searchunlocked = 0;
static int insertlocked = 0;
static int insertunlocked = 0;
static int deletelocked = 0;
static int deleteunlocked = 0;


/*********************************************************************\
 * struct trie_node *new_leaf                                         *
 *        (const char *string, size_t strlen, int32_t ip4_address)    *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   Creates a new node which will be inserted into the trie          *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   string        = The key for the new node.                        *
 *   strlen        = The length of the entire key                     *
 *                       (including total strlen of parents)          *
 *   ip4_address   = The IP Address for the new node.                 *
 *                                                                    *
 * Return:                                                            *
 * ===========                                                        *
 *   *trie_node    = The pointer to the newly created node.           *
 \********************************************************************/
struct trie_node *new_leaf (const char *string, size_t strlen, int32_t ip4_address){
  struct trie_node *new_node = malloc(sizeof(struct trie_node));
  
  // If malloc failed to initialize memory for the new node:
  if (!new_node) {
    printf ("WARNING: Node memory allocation failed.  Results may be bogus.\n");
    return NULL;
  }
  
  // Make sure the strlen is in bounds:
  assert(strlen < 64);
  assert(strlen > 0);
  
  // Load the node with the given arguments:
  new_node->strlen = strlen;
  strncpy(new_node->key, string, strlen);
  new_node->key[strlen] = '\0';
  new_node->ip4_address = ip4_address;

  // Set the default values for the node:
  new_node->next = NULL;
  new_node->children = NULL;
  pthread_mutex_init(&(new_node->mutex), NULL); 
  
  // Return the newly constructed node:
  return new_node;
  
}

void printThreadNumber(){

}

/*********************************************************************\
 * int compare_keys                                                   *
 *        (const char *string1, int len1, const char *string2,        *
 *        int len2, int *pKeylen)                                     *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   See if a key ends with the end substring of another key.         *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   string1       = The first key to compare.                        *
 *   len1          = The length of the first key.                     *
 *   string2       = The second key to compare.                       *
 *   len2          = The length of the second key.                    *
 *   pKeylen       = An output variable. We will store the length of  *
 *                     the compared end substring here                *
 *                                                                    *
 * Return:                                                            *
 * ===========                                                        *
 *   int           = The result of performing on a strncmp on the     *
 *                      last pKeylen bytes of the given strings       *
 \********************************************************************/
int compare_keys (const char *string1, int len1, const char *string2, int len2, int *pKeylen) {
  int keylen, offset1, offset2;

  // Get the shorter of the two key lengths:
  keylen = len1 < len2 ? len1 : len2;

  // Get the offsets for the keys:
  offset1 = len1 - keylen;
  offset2 = len2 - keylen;

  // Verify that the key length is longer than 0:
  assert (keylen > 0);

  // If an output variable was provided, load it with the keylength:
  if (pKeylen)
    *pKeylen = keylen;

  // Return the difference between the first keylen bytes of the strings:
  return strncmp(&string1[offset1], &string2[offset2], keylen);
}



/*********************************************************************\
 * void init(int numthreads)                                          *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   Initialize a brand new trie.                                     *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   numthreads    = The number of threads executing on this tree.    *
 \********************************************************************/
void init(int numthreads) {
  // Notify the user of what he is in for:
  printf("INFO: Initialized with %d Threads!!!  Have Fun!!!\n", numthreads);

  // Create a mutex lock for letting only one insert create a root at a time:
  pthread_mutex_init(&(rootmutex), NULL); 

  // Reset the tree:
  root = NULL;

}



/*********************************************************************\
 * struct trie_node *_search                                          *
 *     (struct trie_node *node, const char *string, size_t strlen)    *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   Recursive helper function. Returns a pointer to the node if      *
 *   found. Stores an optional pointer to the parent, or what should  *
 *   be the parent if not found. See if a key ends with the end       *
 *   substring of another key.                                        *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   node          = The node to begin searching down from.           *
 *   string        = The key to search for.                           *
 *   strlen        = The length of the key.                           *
 *                                                                    *
 * Return:                                                            *
 * ===========                                                        *
 *   *trie_node    = The pointer to the node being searched for.      *
 *                      If not found, the pointer to the parent of    *
 *                      the location where the desired node would     *
 *                      have been placed.                             *
 \********************************************************************/
struct trie_node *_search (struct trie_node *node, const char *string, size_t strlen) {
  int keylen, cmp;

  // First things first, check if we are NULL 
  if (node == NULL){
    return NULL;
  }

  // Make sure the strlen is less than 64:
  assert(node->strlen < 64);

  // Determine if this key is a substring of the string passed in:
  cmp = compare_keys(node->key, node->strlen, string, strlen, &keylen);

  // If the current node is a substring of the string we are looking for:
  if (cmp == 0) {

    // If this key is longer than our search string, the key isn't here
    if (node->strlen > keylen) {

      // We won't go further, so let's give up our lock:
      pthread_mutex_unlock(&(node->mutex));
      searchunlocked++;
      return NULL;

   // If our search string is longer than this node's length:
   } else if (strlen > keylen) {

    // If we have any children below us:
    if(node->children){

      // Let's get ready to move down a level:
      pthread_mutex_lock(&(node->children->mutex));
      searchlocked++;
    }

    // We are done on this level of the tree, let's unlock it:
    pthread_mutex_unlock(&(node->mutex));
    searchunlocked++;
    return _search(node->children, string, strlen - keylen);


  // If we found our search term:
  } else {

    // Verify that it is the right length:
    assert (strlen == keylen);

    // We found it! Unlock it and let's go back:
    pthread_mutex_unlock(&(node->mutex));
    searchunlocked++;
    return node;
  }

  // If the node's key is less than our search key:
  } else if (cmp < 0) {

    // If we have anything else on this level:
    if(node->next){

      // Get ready to move to our neighbor:
      pthread_mutex_lock(&(node->next->mutex));
      searchlocked++;
    }

    // Let's move on to the next node on this level:
    struct trie_node *tn = _search(node->next, string, strlen);

    // We are done on this level, let's unlock our node:
    pthread_mutex_unlock(&(node->mutex));
    searchunlocked++;
    return tn;


  // If the node's key is greater than our search key:
  } else {

    // We are done searching. Let's unlock:
    pthread_mutex_unlock(&(node->mutex));
    searchunlocked++;
    return 0;
  }
}



/*********************************************************************\
 * int search                                                         *
 *     (const char *string, size_t strlen, int32_t *ip4_address)      *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   Wrapper for search function. An end-user should use this.        *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   string        = The key of the node we are looking for.          *
 *   strlen        = The key length of the node we are looking for.   *
 *   ip4_address   = The IP Address of the node we are looking for.   *
 *                                                                    *
 * Return:                                                            *
 * ===========                                                        *
 *   int           = 0: Node Found                                    *
 *                   1: Unable to find a Node                         *
 \********************************************************************/
int search(const char *string, size_t strlen, int32_t *ip4_address){
  struct trie_node *found;

  // Skip strings of length 0
  if (strlen == 0) return 0;

  // If the root has been initialized:
  if(root){

    // Lock the root for us to enter:
    pthread_mutex_lock(&(root->mutex));
    searchlocked++;

    // Search for our search term starting at the root:
    found = _search(root, string, strlen);

    // If we found our search term, get the IP address from the node:
    if (found && ip4_address)
      *ip4_address = found->ip4_address;

    // This is unneeded, but doesn't hurt to make sure all locks are gone after our operation:
    clear_locks();

    // Return the result of our findings:
    return (found != NULL);
  }
  
  // If the root has not been initialized, return 0;
  return 0;
}
  


/*********************************************************************\
 * int _insert                                                        *
 *     (const char *string, size_t strlen, int32_t ip4_address,       *
 *      struct trie_node *node, struct trie_node *parent,             *
 *      struct trie_node *left)                                       *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   Insert a new key, IP, and strlen into the trie.                  *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   *string       = The key to insert in the trie.                   *
 *   strlen        = The length of the key to insert into the trie.   *
 *   ip4_address   = The IP Address to insert into the trie.          *
 *   *node         = The node to insert the new node below.           *
 *   *parent       = The node above this level.                       *
 *   *left         = The previous node in the trie on this level.     *
 *                                                                    *
 * Return:                                                            *
 * ===========                                                        *
 *   int           = 0: Unable to insert Node                         *
 *                   1: Node Successfully inserted                    *
 \********************************************************************/
int _insert(const char *string, size_t strlen, int32_t ip4_address, 
  struct trie_node *node, struct trie_node *parent, struct trie_node *left){
  int cmp, keylen;

  // First things first, check if we are NULL and strlen is in range:
  assert (node != NULL);
  assert (node->strlen < 64);

  // Get the comparison of the first keylen bytes of the keys:
  cmp = compare_keys(node->key, node->strlen, string, strlen, &keylen);

  // If the first keylen bytes are the same:
  if (cmp == 0){
 
    // If we need to insert above this node's level:
    if(node->strlen > keylen) {
      struct trie_node *new_node;

      // Verify some details that must be true:
      assert(keylen == strlen);
      assert((!parent) || parent->children == node);

      // Create a new leaf with the arguments given:
      new_node = new_leaf (string, strlen, ip4_address);
      
      // Calculate the string length:
      node->strlen -= keylen;
      
      // Set the current node as the child of the new node:
      new_node->children = node;

      // Verify that there is either no parent or nothing to the left:
      assert ((!parent) || (!left));

      // If this is not the root level:
      if (parent) {

        // Set the parent's child to the new_node:
        parent->children = new_node;

      // If there is a node to the left of this node:
      } else if (left) {

        // Set the left node's next node to the new_node:  
        left->next = new_node;

      // If this node was the root:
      } else if ((!parent) || (!left)) {

        // Set the root to this node:
        root = new_node;
      }

      // Unlock the new node and prepare to return:
      pthread_mutex_unlock(&(node->mutex));
      insertunlocked++;

      // Return Success!
      return 1;

    // If we need to insert below this node's level:
    } else if (strlen > keylen) {

      // If this node has no children:
      if (node->children == NULL) {

        // Create a new node with the given arguments:
        struct trie_node *new_node = new_leaf (string, strlen - keylen, ip4_address);

        // Set this node's child to be the new node:
        node->children = new_node;

        // Unlock the new node and prepare to return:
        pthread_mutex_unlock(&(node->mutex));
        insertunlocked++;

        // Return Success!
        return 1;

      // If this node already has children:
      } else {

        pthread_mutex_lock(&(node->children->mutex));
        insertlocked++;

        // Recur on children list, store "parent" (loosely defined)
        int result = _insert(string, strlen - keylen, ip4_address,
          node->children, node, NULL);

        pthread_mutex_unlock(&(node->mutex));
        insertunlocked++;

        return result;
      }

    // If this node's key is the key we are looking for:
    } else {

      // Verify needed values:
      assert (strlen == keylen);

      // If the node's IP address is not defined:
      if (node->ip4_address == 0) {

        // Set this node's IP address to this given argument:
        node->ip4_address = ip4_address;

        // Finished inserting, unlock the current node:
        pthread_mutex_unlock(&(node->mutex));
        insertunlocked++;

        // Return Success!
        return 1;

      // If the node is already defined:
      } else {

        // TODO: Handle Squatting:
        node->num_squatters++;

        // Finished inserting, unlock the current node:
        pthread_mutex_unlock(&(node->mutex));
        insertunlocked++;

        // Return Failure!
        return 0;
      }
    }

  // If the first keylen bytes aren't the same:
  } else {

    //  Determine if there is there any common substring:
    int i, cmp2, keylen2, overlap = 0;

    // For all character's in the key:
    for (i = 1; i < keylen; i++) {

      // See if the node's key shares a substring with the given key:
      cmp2 = compare_keys (&node->key[i], node->strlen - i, 
        &string[i], strlen - i, &keylen2);

      // Verify the keylength is greater than 0:
      assert (keylen2 > 0);

      // If there is a common substring:
      if (cmp2 == 0) {

      // Set overlap to 1 and break out of the loop:
      overlap = 1;
      break;
    }
  }

  // If the key's end in a common substring:
  if (overlap) {

    // Create a new node to store the common parent node:
    struct trie_node *new_node = new_leaf (&string[i], strlen - i, 0);

    // Lock the new node so we can work with it atomically:
    pthread_mutex_lock(&(new_node->mutex));
    insertlocked++;

    // Calculate the difference in strlen from the parent to the node:
    int diff = node->strlen - i;

    // Verify the difference is a valid value and store it:
    assert ((node->strlen - diff) > 0);
    node->strlen -= diff;

    // Set the common parent's child to the current node:
    new_node->children = node;

    // Verify needed details:
    assert ((!parent) || (!left));

    // If the current node was the root:
    if (node == root) {

      // Swap the common parent into the current node's old location:
      new_node->next = node->next;
      node->next = NULL;
      root = new_node;

    // If the current node had a parent:
    } else if (parent) {

      // Insert the common parent between the current node and old parent:
      assert(parent->children == node);
      new_node->next = NULL;
      parent->children = new_node;

    // If the current node had a left neighbor:
    } else if (left) {

      // Swap the common parent into the current node's old location:
      new_node->next = node->next;
      node->next = NULL;
      left->next = new_node;

    // If the current node had no parent and no left neighbor:
    } else if ((!parent) && (!left)) {

      // Set the common parent as the root:
      root = new_node;
    }

    // Try inserting the value again now that it has a place:
    int result = _insert(string, i, ip4_address, node, new_node, NULL);

    // Unlock the new node and prepare to return:
    pthread_mutex_unlock(&(new_node->mutex));
    insertunlocked++;

    // Return the result of the insertion:
    return result;


  // If the new node belongs to the right of the current node:
  } else if (cmp < 0) {

    // If the current node is the last in line on this level:
    if (node->next == NULL) {

      // Create a new node to add to the right:
      struct trie_node *new_node = new_leaf (string, strlen, ip4_address);

      // Place it next in line on this level:
      node->next = new_node;

      // Unlock the new node and prepare to return:
      pthread_mutex_unlock(&(node->mutex));
      insertunlocked++;

      // Return Success!
      return 1;

    // If the current node is not the last in line on this level:
    } else {

      if(node->next){
        // Lock the next node so we can hop into it:
        pthread_mutex_lock(&(node->next->mutex));
        insertlocked++;
      }

      // Recur right (the node's key is "greater" than  the search key)
      int result = _insert(string, strlen, ip4_address, node->next, NULL, node);

      // Unlock the new node and prepare to return:
      pthread_mutex_unlock(&(node->mutex));
      insertunlocked++;

      // Return the result of traversing right:
      return result;
    }

  // If the new node belongs at this location:
  } else {

    // Create the new node:
    struct trie_node *new_node = new_leaf (string, strlen, ip4_address);

    pthread_mutex_lock(&(new_node->mutex));
    insertlocked++;

    // Set the new_node's next to the current node:
    new_node->next = node;

    // If the current node was the root, set the new_node to the root:
    if (node == root)
      root = new_node;

    // If the current node had a parent, set the parent's child to the new_node:
    else if (parent && parent->children == node)
      parent->children = new_node;

    // Unlock the current node and prepare:
    pthread_mutex_unlock(&(node->mutex));
    insertunlocked++;

    // Unlock the new node and prepare to return:
    pthread_mutex_unlock(&(new_node->mutex));
    insertunlocked++;

    // Return Success!
    return 1;
  }

  // Unlock the new node and prepare to return:
  pthread_mutex_unlock(&(node->mutex));
  insertunlocked++;

  // Return Success!
  return 1;
  }
}



/*********************************************************************\
 * int insert(const char *string, size_t strlen, int32_t ip4_address) *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   Wrapper for insert function. An end-user should use this.        *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   string        = The key of the node we are inserting.            *
 *   strlen        = The key length of the node we are inserting.     *
 *   ip4_address   = The IP Address of the node we are inserting.     *
 *                                                                    *
 * Return:                                                            *
 * ===========                                                        *
 *   int           = 0: Node Inserted                                 *
 *                   1: Unable to inserted a Node                     *
 \********************************************************************/
int insert (const char *string, size_t strlen, int32_t ip4_address) {
  int result;

  // Skip strings of length 0
  if (strlen == 0) return 0;

  pthread_mutex_lock(&rootmutex);

  // If the root hasn't been defined yet:
  if (root == NULL) {

    printf("Creating root: \n");
    // Create the root and return Success!
    root = new_leaf(string, strlen, ip4_address);

    pthread_mutex_unlock(&rootmutex);
    return 1;
  } pthread_mutex_unlock(&rootmutex);

  pthread_mutex_lock(&(root->mutex));
  insertlocked++;

  // Insert the new node:
  result = _insert (string, strlen, ip4_address, root, NULL, NULL);

  // This is unneeded, but doesn't hurt to make sure all locks are gone after our operation:
  clear_locks();

  return result;
}



/*********************************************************************\
 * struct trie_node *_delete (struct trie_node *node,                 *
 *                                 const char *string, size_t strlen) *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 * Recursive helper function. Returns a pointer to the node if found. *
 * Stores an optional pointer to the parent, or what should be the    *
 * parent if not found.                                               *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   *node         = The node to start looking from.                  *
 *   *string       = The key that we want to delete from the trie.    *
 *   strlen        = The length of the key we want to delete.         *
 *                                                                    *
 * Return:                                                            *
 * ===========                                                        *
 *   *trie_node    = The pointer to the node we deleted.              *
 *                                                                    *
 \********************************************************************/
struct trie_node *_delete (struct trie_node *node, const char *string, size_t strlen){
  int keylen, cmp;

  // Check if we received NULL:
  if (node == NULL) return NULL;

  // Verify that the strlen provided is less than 64:
  assert(node->strlen < 64);

  // See if this key is a substring of the string passed in:
  cmp = compare_keys (node->key, node->strlen, string, strlen, &keylen);
  
  // If the node contains a substring of the node we want to delete:
  if (cmp == 0) {

    // If this key is longer than our search string, the key can't be here
    if (node->strlen > keylen) {

      // Unlock the node and prepare to return:
      pthread_mutex_unlock(&(node->mutex));
      deleteunlocked++;

      // Return NULL because we didn't do anything:
      return NULL;
    
    // If this key is shorter than our search string, the key is below us:
    } else if (strlen > keylen) {

      if(node->children){
        // Lock the child node and prepare to go deeper:
        pthread_mutex_lock(&(node->children->mutex));
        deletelocked++;
      }

      // Attempt to delete the node starting from the child of this node:
      struct trie_node *found =  _delete(node->children, string, strlen - keylen);
    
      // If the node was found below this level:
      if (found){

        // If the found node has no children and has no IP address:
        if (found->children == NULL && found->ip4_address == 0){
          assert(node->children == found);

          // Set this node's child to the next in line from the found node:
          node->children = found->next;

          // Unlock the node and prepare to free it:
          pthread_mutex_unlock(&(found->mutex));
          deleteunlocked++;

          // Delete and free the found node:
          free(found);
        }

        // If the current node is the root, has no children, and has no IP address:
        if (node == root && node->children == NULL && node->ip4_address == 0) {

          // Set the root node to it's neighbor:
          root = node->next;

          // Unlock the node and prepare to return:
          pthread_mutex_unlock(&(node->mutex));
          deleteunlocked++;

          // Free this node:
          free(node);
        }

        // Return the location of the deleted node:
        return node;
    
      // If the node was not found below this level:
      } else {

        // Unlock the node and prepare to return:
        pthread_mutex_unlock(&(node->mutex));
        deleteunlocked++;

        // Return NULL:
        return NULL;
      }

    // If the key is the same length as our node to be deleted:
    } else {
      assert (strlen == keylen);

      // If the node represents a DNS entry and not just a shared path:
      if (node->ip4_address){
     
        // Erase the IP address of the node:
        node->ip4_address = 0;

        // If the node is the root with no children:
        if (node == root && node->children == NULL && node->ip4_address == 0) {
      
          // Set the root to the next on the level:
          root = node->next;
      
          // Unlock the node and prepare to return:
          pthread_mutex_unlock(&(node->mutex));
          deleteunlocked++;

          // Free the current node from memory:
          free(node);

          // Return a garbage pointer representing deletion of root node:
          return (struct trie_node *) 0x100100; 
        }
      
        // Return the node where we deleted the IP address:
        return node;

      // If the node represents a a shared path with no IP address:
      } else {

        // Unlock the node and prepare to return:
        pthread_mutex_unlock(&(node->mutex));
        deleteunlocked++;

        // Return NULL because we didn't do anything:
        return NULL;
      }
    }

  // If our search term is to the right of this node:
  } else if (cmp < 0) {

    if(node->next){
      // Lock the next node:
      pthread_mutex_lock(&(node->next->mutex));
      deletelocked++;
    }

    // Create a node that will look to our right for us:
    struct trie_node *found = _delete(node->next, string, strlen);

    // If the node was found:
    if (found) {

      // If the found node has no children and has no IP address:
      if (found->children == NULL && found->ip4_address == 0) {
        assert(node->next == found);

        // Set this node's next to the found node's next:
        node->next = found->next;

        // Unlock the node and prepare to return:
        pthread_mutex_unlock(&(found->mutex));
        deleteunlocked++;

        // Free the found node:
        free(found);
      }       

      // Return this node:
      return node;
    }

    // Unlock the node and prepare to return:
    pthread_mutex_unlock(&(node->mutex));
    deleteunlocked++;

    // Return NULL because we didn't do anything:
    return NULL;
  
  // If our search term isn't below us, to the right of us, or this node:
  } else {
  
    // Unlock the node and prepare to return:
    pthread_mutex_unlock(&(node->mutex));
    deleteunlocked++;

    // Return NULL because we didn't do anything:
    return NULL;
  }
}



/*********************************************************************\
 * int delete (const char *string, size_t strlen)                     *
 *====================================================================*
 * Desc:                                                              *
 * ===========                                                        *
 *   Wrapper for delete function. An end-user should use this.        *
 *                                                                    *
 * Parameters:                                                        *
 * ===========                                                        *
 *   string        = The key of the node we are deleting.             *
 *   strlen        = The key length of the node we are deleting.      *
 *                                                                    *
 * Return:                                                            *
 * ===========                                                        *
 *   int           = 0: Unable to delete Node                         *
 *                   1: Successfully Deleted Node                     *
 \********************************************************************/
int delete (const char *string, size_t strlen){
  // Skip strings of length 0
  if (strlen == 0) return 0;

  //return 0;

  // If the root is not NULL:
  if(root){

    pthread_mutex_lock(&(root->mutex));
    deletelocked++;

    // Attempt to delete the Node given:
    int result = (NULL != _delete(root, string, strlen));

    // This is unneeded, but doesn't hurt to make sure all locks are gone after our operation:
    clear_locks();

    // Return the result of our attempt:
    return result;
  }
  
  // If the root is NULL, return Failure:
  return 0;
}


void _print (struct trie_node *node) {
  printf ("Node at %p.  Key %.*s, IP %d.  Next %p, Children %p\n", 
    node, node->strlen, node->key, node->ip4_address, node->next, node->children);

  if (node->children){
    _print(node->children);
  }

  if (node->next)
    _print(node->next);
}

void print() {
  printf ("Root is at %p\n", root);

  /* Do a simple depth-first search */
  if (root)
    _print(root);
}


void _clear_locks(struct trie_node *node){
  pthread_mutex_trylock(&(node->mutex));
  pthread_mutex_unlock(&(node->mutex));

  if (node->children){
    _clear_locks(node->children);
  }

  if (node->next)
    _clear_locks(node->next);
}



void clear_locks() {
  if (root)
    _clear_locks(root);
}
