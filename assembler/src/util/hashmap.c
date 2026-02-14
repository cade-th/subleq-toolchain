#include "hashmap.h"
#include <stdlib.h>

// this will basically just be an dynamic array where we 
// index using a string instead of a number
// this function works by allocating memory for the header and 
// 2 items (from the initial capacity), and it increments the 
// pointer at the header in order to go to the first item,
// and then returns that pointer as a void pointer then is
// then casted to an input type in order to make this data
// structure generic
void *hash_map_init(int item_size, int capacity) {
   void *ptr = 0; 
    int size = item_size * capacity + sizeof(Hash_Header);
    Hash_Header *h = malloc(size);

    if (h) {
        h->capacity = capacity;
        h->length = 0;
        ptr = h + 1;
    }

    return ptr;
}


