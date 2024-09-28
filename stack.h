#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "define.h"

typedef enum {
    NO_ERRORS                       = 0x00000000,
    MEMORY_ALLOCATION_ERROR         = 0x00000001,
    MEMORY_REALLOCATION_ERROR       = 0x00000002,
    ELEMENT_WIDTH_ERROR             = 0x00000004,
    NO_ELEMNTS_TO_POP_ERROR         = 0x00000010,
    NULL_CAPACITY_ERROR             = 0x00000020,
    INVALID_SIZE_ERROR              = 0x00000040,
    DATA_INVALID_POINTER_ERROR      = 0x00000100,
    NULL_ELEMENT_WIDTH_ERROR        = 0x00000200,
    STACK_ALREADY_INITIALIZED_ERROR = 0x00000400,
    CAPACITY_LIMIT_EXCEED_ERROR     = 0x00001000,

#ifdef ON_CANARY_PROTECT
    LEFT_CANARY_PROTECT_FAILURE     = 0x00002000,
    RIGTH_CANARY_PROTECT_FAILURE    = 0x00004000,
    DATA_CANARY_PROTECT_FAILURE     = 0x00010000,
#endif /* ON_CANARY_PROTECT */

#ifdef ON_HASH_PROTECT
    HASH_PROTECTION_FAILED          = 0x00020000,
#endif /* ON_HASH_PROTECT */
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
#ifdef ON_CANARY_PROTECT
typedef unsigned long long canary_t;
const canary_t RIGTH_CANARY_MASK = 0xFBADBEEF;
const canary_t LEFT_CANARY_MASK  = 0xC0FFEE;
const canary_t DATA_CANARY       = 0x3DAD;
#endif /* ON_CANARY_PROTECT */
#ifdef ON_HASH_PROTECT
typedef uint32_t hash_t;
#endif /* ON_HASH_PROTECT */

typedef struct {
#ifdef ON_CANARY_PROTECT
    canary_t left_canary;
#endif /* ON_CANARY_PROTECT */
    void* data;
    size_t elm_width;
    size_t size;
    size_t capacity;
    size_t base_capacity;
    error_t error;
#ifdef DEBUG
    location_info_t location_info;
#endif /* DEBUG */
#ifdef ON_CANARY_PROTECT
    canary_t rigth_canary;
#endif /* ON_CANARY_PROTECT */
#ifdef ON_HASH_PROTECT
    hash_t hash_stack;
    hash_t hash_data;
#endif
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

#ifdef ON_HASH_PROTECT
void set_stack_hash(my_stack_t* stk);
hash_t stack_hash(my_stack_t* stk, hash_t* new_hash_data, hash_t* new_hash_stack);
hash_t stack_hash_counter(const void* buffer, size_t buffer_bytes_amount);
#endif /* ON_HASH_PROTECT */

#endif /* STACK_H */

