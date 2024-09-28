#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "define.h"

typedef enum {
    NO_ERRORS                       = 0x00000001,
    MEMORY_ALLOCATION_ERROR         = 0x00000002,
    MEMORY_REALLOCATION_ERROR       = 0x00000004,
    ELEMENT_WIDTH_ERROR             = 0x00000010,
    NO_ELEMNTS_TO_POP_ERROR         = 0x00000020,
    NULL_CAPACITY_ERROR             = 0x00000040,
    INVALID_SIZE_ERROR              = 0x00000100,
    DATA_INVALID_POINTER_ERROR      = 0x00000200,
    NULL_ELEMENT_WIDTH_ERROR        = 0x00000400,
    STACK_ALREADY_INITIALIZED_ERROR = 0x00001000,
    CAPACITY_LIMIT_EXCEED_ERROR     = 0x00002000
} error_t;

typedef enum {
    EXPAND = 0,
    SHRINK = 1
} mem_modify_t;

#ifdef DEBUG
    typedef struct {
        const char* name;
        const char* file;
        size_t line;
        const char* func;
    } location_info_t;
#endif /* DEBUG */

typedef struct {
    void* data;
    size_t elm_width;
    size_t size;
    size_t capacity;
    size_t base_capacity;
    error_t error;

#ifdef DEBUG
    location_info_t location_info;
#endif /* DEBUG */
} my_stack_t;

error_t stack_ctor(my_stack_t* stk, size_t size, size_t base_capacity ON_DEBUG(, location_info_t location_info));
error_t stack_dtor(my_stack_t* stk);
error_t stack_resize(my_stack_t* stk, mem_modify_t mode);

error_t stack_push(my_stack_t* stk, const void* elm);
error_t stack_pop(my_stack_t* stk, void* elm);

void set_stack_dump_ostream(FILE* ostream);
error_t stack_dump(const my_stack_t* stk, const char* file, size_t line, const char* func);
error_t stack_error(my_stack_t* stk);
error_t stack_assert(my_stack_t* stk, const char* file, size_t line, const char* func);
#endif /* STACK_H */
