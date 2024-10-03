#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "define.h"

//------------------------------------------------------------------------------------------------

ON_HASH_PROTECT(typedef uint32_t hash_t;)
typedef void (*print_t) (void* elm, FILE* ostream);

const size_t CAPACITY_COEFF = 2;
const size_t MIN_CAPACITY = 1;
const size_t MAX_CAPACITY = 1e6;
const uint64_t POISON_VALUE = 0xDEADDEADDEADDEAD;

//------------------------------------------------------------------------------------------------

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
    NON_VALID_POINTER_ERROR         = 0x00002000,

#ifdef CANARY_PROTECT
    LEFT_CANARY_PROTECT_FAILURE     = 0x00004000,
    RIGTH_CANARY_PROTECT_FAILURE    = 0x00010000,
    DATA_CANARY_PROTECT_FAILURE     = 0x00020000,
#endif /* CANARY_PROTECT */

#ifdef HASH_PROTECT
    HASH_PROTECTION_FAILED          = 0x00040000,
#endif /* HASH_PROTECT */
} error_t;

typedef enum {
    NONE    = 0x00,
    READ    = 0x01,
    WRITE   = 0x02,
    EXECUTE = 0x04,
    DEFAULT = READ | WRITE,
    ALL = READ | WRITE | EXECUTE,
} ptr_protect_t;

typedef enum {
    EXPAND       = 0,
    SQUEEZE      = 1,
    SHINK_TO_FIT = 2,

} mem_modify_t;

//------------------------------------------------------------------------------------------------

#ifdef DEBUG
    typedef struct {
        const char* stack_name;
        const char* file;
        size_t line;
        const char* func;
    } location_info_t;
#endif /* DEBUG */

#ifdef CANARY_PROTECT
typedef unsigned long long canary_t;
const canary_t LEFT_CANARY_MASK  = 0xC0FFEE;
const canary_t RIGTH_CANARY_MASK = 0xFBADBEEF;
const canary_t DATA_CANARY       = 0x3DAD;
#endif /* CANARY_PROTECT */

//------------------------------------------------------------------------------------------------

typedef struct {
#ifdef CANARY_PROTECT
    canary_t left_canary;
#endif /* CANARY_PROTECT */
    void* data;
    size_t elm_width;
    size_t size;
    size_t capacity;
    size_t base_capacity;
    error_t error;
    uint64_t poison_value;
    char* poison_value_buffer;
    print_t print;
#ifdef __APPLE__
    ptr_protect_t data_permissions;
#endif /* __APPLE__ */
#ifdef DEBUG
    location_info_t location_info;
#endif /* DEBUG */
#ifdef CANARY_PROTECT
    canary_t rigth_canary;
#endif /* CANARY_PROTECT */
#ifdef HASH_PROTECT
    hash_t hash_stack;
    hash_t hash_data;
#endif /* HASH_PROTECT */
} my_stack_t;

//------------------------------------------------------------------------------------------------

error_t stack_ctor(my_stack_t* stk, size_t elm_size, size_t base_capacity, print_t print
                   ON_DEBUG(, location_info_t location_info));
error_t stack_dtor(my_stack_t* stk);
error_t stack_resize(my_stack_t* stk, mem_modify_t mode);

#endif /* STACK_H */

