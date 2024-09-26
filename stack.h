#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

typedef enum {
    NO_ERRORS = 0,
    MEMORY_ALLOCATION_ERROR = 1,
    MEMORY_REALLOCATION_ERROR = 2,
    ELEMENT_WIDTH_ERROR = 3,
    NO_ELEMNTS_TO_POP_ERROR = 4,
    NULL_CAPACITY_ERROR = 5,
    INVALID_SIZE_ERROR = 6,
    DATA_INVALID_POINTER_ERROR = 7,
    NULL_ELEMENT_WIDTH_ERROR = 8,
    STACK_ALREADY_INITIALIZED_ERROR = 9
} error_t;

typedef enum {
    EXPAND = 0,
    SHRINK = 1
} mem_modify_t;

typedef struct {
    void* data;
    size_t elm_width;
    size_t size;
    size_t capacity;
    size_t base_capacity;
    error_t error;
} my_stack_t;

error_t stack_ctor(my_stack_t* stk, size_t size, size_t base_capacity);
error_t stack_dtor(my_stack_t* stk);

error_t stack_push(my_stack_t* stk, const void* elm);
error_t stack_pop(my_stack_t* stk, void* elm);

error_t stack_assert(my_stack_t* stk, const char* file, size_t line);
error_t stack_dump(my_stack_t* stk, const char* file, size_t line);
void set_stack_dump_ostream(FILE* ostream);
error_t stack_error(my_stack_t* stk);
error_t stack_resize(my_stack_t* stk, mem_modify_t mode);
#endif /* STACK_H */
