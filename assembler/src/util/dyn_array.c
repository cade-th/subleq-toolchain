#include "dyn_array.h"
#include <stdlib.h>
#include <string.h>

void *dyn_array_init(int item_size, int capacity) {
    void *ptr = 0;
    int size = item_size * capacity + sizeof(Array_Header);
    Array_Header *h = malloc(size);

    if(h) {
	h->capacity = capacity;
	h->length = 0;
	ptr = h + 1;
    }

    return ptr;
}
void *array_ensure_capacity(void *a, int item_count, int item_size) {
    Array_Header *h = array_header(a);
    int desired_capacity = h->length + item_count;

    if (h->capacity < desired_capacity) {
        int new_capacity = h->capacity * 2;
        while (new_capacity < desired_capacity) {
            new_capacity *= 2;
        }

        size_t new_size = sizeof(Array_Header) + new_capacity * item_size;
        Array_Header *new_h = malloc(new_size);

        if (new_h) {
            size_t old_size = sizeof(*h) + h->length * item_size;
            memcpy(new_h, h, old_size);

            /* fix this at some point
            if (h->a->free) {
                h->a->free(old_size, h, h->a->context);
            }
            */

            new_h->capacity = new_capacity;
            h = new_h + 1;
        } else {
            h = 0;
        }
    } else { h += 1; }

    return h;
}

