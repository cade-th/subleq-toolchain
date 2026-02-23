#pragma once

typedef struct {
	int length;
	int capacity;
} Array_Header;

#define INITIAL_CAPACITY 2

#define DYN_ARRAY(T) (T *)dyn_array_init(sizeof(T), INITIAL_CAPACITY)

#define ARRAY_PUSH(a,v) ( \
    (a) = array_ensure_capacity(a, 1, sizeof(v)), \
    (a)[array_header(a)->length] = (v), \
    &(a)[array_header(a)->length++]) 

#define array_header(a) ((Array_Header *)(a) - 1)
#define ARRAY_LENGTH(a) (array_header(a)->length)
#define array_capacity(a) (array_header(a)->capacity)

void *dyn_array_init(int item_size, int capacity);
void *array_ensure_capacity(void *a, int item_count, int item_size);

	
