#include "hashmap.h"

// this will basically just be an dynamic array where we 
// index using a string instead of a number
// this function works by allocating memory for the header and 
// 2 items (from the initial capacity), and it increments the 
// pointer at the header in order to go to the first item,
// and then returns that pointer as a void pointer then is
// then casted to an input type in order to make this data
// structure generic
unsigned int hash(char *string) {
   unsigned long hash = 5381;
   int c;
   while ((c = *string++)) {
      // hash * 33 + c
      hash = ((hash << 5) + hash) + c;
   }
   return hash;
}



