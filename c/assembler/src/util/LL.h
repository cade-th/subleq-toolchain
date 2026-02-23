#pragma once

// poor mans polymorphism for a generic linked list
#define LL(type) \
    typedef struct type##_Node { \
        type data; \
        struct type##_Node *next; \
    } type##_Node; \
    \
    /* Push to the front (O(1)) */ \
    void type##_push(type##_Node **head, type value) { \
        type##_Node *new_node = malloc(sizeof(type##_Node)); \
        new_node->data = value; \
        new_node->next = *head; \
        *head = new_node; \
    } \
    \
    /* Pop from the front (O(1)) */ \
    type type##_pop(type##_Node **head) { \
        if (*head == NULL) return (type)0; \
        type##_Node *temp = *head; \
        type value = temp->data; \
        *head = temp->next; \
        free(temp); \
        return value; \
    } \

   // usage
// LL(T)
// manually write
/*
    int_Node *my_ints = NULL;
    int_push(&my_ints, 10);
    int_push(&my_ints, 20);
    printf("Popped int: %d\n", int_pop(&my_ints)); // Prints 20
*/
