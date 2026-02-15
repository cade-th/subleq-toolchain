#pragma once

#include "dyn_array.h"
#include <string.h>

#define HASH_CAPACITY 256

// 1. Find the bucket (The Dynamic Array)
#define HASH_GET_BUCKET(h, s) ((h)[hash(s) % HASH_CAPACITY])

// 2. Push with Lazy Initialization
#define HASH_PUSH(h, s, v, T) do { \
    int _idx = hash(s) % HASH_CAPACITY; \
    if ((h)[_idx] == NULL) { \
        (h)[_idx] = (T *)dyn_array_init(sizeof(T), INITIAL_CAPACITY); \
    } \
    ARRAY_PUSH((h)[_idx], v); \
} while(0)

// Updated HASH_FIND with Type (T) parameter
#define HASH_FIND(h, s, out_ptr, T) do { \
    T* _bucket = (T*)HASH_GET_BUCKET(h, s); \
    (out_ptr) = NULL; \
    if (_bucket) { \
        for (int _i = 0; _i < ARRAY_LENGTH(_bucket); _i++) { \
            if (strcmp(_bucket[_i].key, (s)) == 0) { \
                (out_ptr) = &(_bucket[_i]); \
                break; \
            } \
        } \
    } \
} while(0)

// hash function
int hash(char *string);
