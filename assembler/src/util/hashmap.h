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

#define HASH_FIND(h, s, T) ({ \
    T* _result = NULL; \
    T* _bucket = (T*)HASH_GET_BUCKET(h, s); \
    if (_bucket) { \
        for (int _i = 0; _i < ARRAY_LENGTH(_bucket); _i++) { \
            if (strcmp(_bucket[_i].key, (s)) == 0) { \
                _result = &(_bucket[_i]); \
                break; \
            } \
        } \
    } \
    _result; \
})

// hash function
int hash(char *string);
