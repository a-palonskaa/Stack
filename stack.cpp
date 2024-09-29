#include <time.h>

#include "stack.h"
#include "define.h"

static void print_time(FILE *ostream);
static const char* stack_error_message(error_t error);
static FILE** stack_dump_ostream();

const size_t CAPACITY_COEFF = 2;
const size_t MIN_CAPACITY = 1;
const size_t MAX_CAPACITY = 1e6;

static FILE** stack_dump_ostream() {
    static FILE* out = nullptr;
    return &out;
}

void set_stack_dump_ostream(FILE* ostream) {
    *stack_dump_ostream() = ostream;
}

error_t stack_ctor(my_stack_t* stk, size_t elm_size, size_t base_capacity
                   ON_DEBUG(, location_info_t location_info)) {
    assert(base_capacity != 0);

    if (stack_error(stk) == NO_ERRORS) {
        STACK_DUMP_(stk);
        return STACK_ALREADY_INITIALIZED_ERROR;
    }

#ifdef DEBUG
    stk->location_info.stack_name = location_info.stack_name;
    stk->location_info.file = location_info.file;
    stk->location_info.line = location_info.line;
    stk->location_info.func = location_info.func;
#endif /* DEBUG */

#ifdef ON_CANARY_PROTECT
    stk->left_canary  = LEFT_CANARY_MASK  ^ (canary_t) stk;
    stk->rigth_canary = RIGTH_CANARY_MASK ^ (uint64_t) stk;
#endif /* ON_CANARY_PROTECT */

    stk->error = NO_ERRORS;

    if (!(stk->elm_width = elm_size)) {
        stk->error = ELEMENT_WIDTH_ERROR;
        STACK_DUMP_(stk);
        return ELEMENT_WIDTH_ERROR;
    }

    STATIC_ASSERT(MIN_CAPACITY > 0, Minimum_capacity_should_be_greater_than_0);
    stk->capacity = base_capacity > MIN_CAPACITY ?
                    base_capacity : MIN_CAPACITY;

#ifdef ON_CANARY_PROTECT
    if (!(stk->data = calloc(stk->capacity * stk->elm_width + 2 * sizeof(canary_t), sizeof(char)))) {
        stk->error = MEMORY_ALLOCATION_ERROR;
        STACK_DUMP_(stk);
        return MEMORY_ALLOCATION_ERROR;
    };

    memcpy(stk->data, &DATA_CANARY, sizeof(canary_t));
    stk->data = (void*) ((char*) stk->data + sizeof(canary_t));
    memcpy((char*) stk->data + stk->capacity * stk->elm_width, &DATA_CANARY, sizeof(canary_t));
#else
    if (!(stk->data = calloc(stk->capacity, stk->elm_width))) {
        stk->error = MEMORY_ALLOCATION_ERROR;
        STACK_DUMP_(stk);
        return MEMORY_ALLOCATION_ERROR;
    };
#endif /* ON_CANARY_PROTECT */

#ifdef ON_HASH_PROTECT
    set_stack_hash(stk);
#endif /* ON_HASH_PROTECT */

    STACK_ASSERT_(stk);
    STACK_DUMP_(stk);
    return NO_ERRORS;
}

error_t stack_dtor(my_stack_t* stk) {
    assert(stk != nullptr);
#ifdef ON_CANARY_PROTECT
    free((char*) stk->data - sizeof(canary_t));
#else
    free(stk->data);
#endif /* ON_CANARY_PROTECT */
    stk->data = nullptr;
    stk->size = 0;
    stk->elm_width = 0;
    stk->capacity = 0;
    stk->error = NO_ERRORS;
#ifdef ON_HASH_PROTECT
    set_stack_hash(stk);
#endif /* ON_HASH_PROTECT */
    STACK_DUMP_(stk);
    return NO_ERRORS;
}

error_t stack_push(my_stack_t* stk, const void* elm) {
    assert(stk != nullptr);
    assert(elm != nullptr);
    STACK_ASSERT_(stk);

    if (stk->size == stk->capacity) {
        if (stk->capacity * CAPACITY_COEFF >= MAX_CAPACITY) {
            stk->error = CAPACITY_LIMIT_EXCEED_ERROR;
            return CAPACITY_LIMIT_EXCEED_ERROR;
        }
        if (stack_resize(stk, EXPAND) != NO_ERRORS) {
            STACK_DUMP_(stk);
            return stk->error;
        }
    }

    memcpy(((char*) stk->data + stk->size * stk->elm_width), elm, stk->elm_width);
    stk->size++;
#ifdef ON_HASH_PROTECT
    set_stack_hash(stk);
#endif /* ON_HASH_PROTECT */
    STACK_DUMP_(stk);
    return NO_ERRORS;
}

error_t stack_pop(my_stack_t* stk, void* elm) {
    assert(stk != nullptr);
    assert(elm != nullptr);
    STACK_ASSERT_(stk);

    if (stk->size == 0)  {
        stk->error = NO_ELEMNTS_TO_POP_ERROR;
        return NO_ELEMNTS_TO_POP_ERROR;
    }

    memcpy(elm, (char*) stk->data + (stk->size - 1) * stk->elm_width, stk->elm_width);
    stk->size--;

    size_t new_capacity = stk->capacity / (CAPACITY_COEFF * CAPACITY_COEFF);
    if (stk->size <=  new_capacity && new_capacity > stk->base_capacity) {
        if (stack_resize(stk, SHRINK) != NO_ERRORS) {
            STACK_DUMP_(stk);
            return stk->error;
        }
    }
#ifdef ON_HASH_PROTECT
    set_stack_hash(stk);
#endif /* ON_HASH_PROTECT */
    STACK_DUMP_(stk);
    return NO_ERRORS;
}

error_t stack_assert(my_stack_t* stk, const char* file, size_t line, const char* func) {
    if (stack_error(stk) != NO_ERRORS && stk->error != NO_ELEMNTS_TO_POP_ERROR) {
        stack_dump(stk, file, line, func);
        assert(0);
    }
    return NO_ERRORS;
}

error_t stack_dump(const my_stack_t* stk, const char* file, size_t line, const char* func) {
    assert(stk != nullptr);
    assert(file != nullptr);
    assert(line > 0);

    FILE* ostream = *stack_dump_ostream();
    if (ostream == nullptr) {
        ostream = stdout;
    }
    fprintf(ostream, "stack_t [%p]\n", stk);
    print_time(ostream);
    fprintf(ostream, "\n");
    fprintf(ostream, "called from %s:%zu(%s)\n", file, line, func);
#ifdef DEBUG
    fprintf(ostream, "name ""%s"" born at %s:%zu(%s)\n",
                     stk->location_info.stack_name, stk->location_info.file,
                     stk->location_info.line, stk->location_info.func);
#endif
    fprintf(ostream, "\n[ERROR STATUS]: ");
    fprintf(ostream, "%s\n", stack_error_message(stk->error));

    fprintf(ostream, "element width = %zu\n", stk->elm_width);
    fprintf(ostream, "capacity = %zu\n", stk->capacity);
    fprintf(ostream, "size = %zu\n\n", stk->size);

    fprintf(ostream, "data[%p]\n{\n", stk->data);
    if (!is_data_nullptr) {
        for (size_t i = 0; i < stk->size; i++) {
            fprintf(ostream, "*[%08zX] = ", i);
            for (ssize_t j = (ssize_t) stk->elm_width - 1; j >= 0; j--) {
                fprintf(ostream, "%X ", *((char*) stk->data + i * stk->elm_width + j));
            }
            fprintf(ostream, "\n");
        }
        fprintf(ostream, "}\n\n");
    }
#ifdef ON_CANARY_PROTECT
    fprintf(ostream, "LEFT  STACK CANARY PROTECT: \n\t%llX(without mask)\n\t%llx(with mask)\n",
                     stk->left_canary,  stk->left_canary  ^ (canary_t) stk);
    fprintf(ostream, "RIGHT STACK CANARY PROTECT: \n\t%llX(without mask)\n\t%llx(with mask)\n\n",
                     stk->rigth_canary, stk->rigth_canary ^ (canary_t) stk);

    if (stk->data) {
        canary_t rigth_canary = 0;
        canary_t left_canary  = 0;

        memcpy(&left_canary, (char*) stk->data - sizeof(canary_t), sizeof(canary_t));
        memcpy(&rigth_canary, (char*) stk->data + stk->capacity * stk->elm_width, sizeof(canary_t));

        fprintf(ostream, "LEFT DATA CANARY PROTECT:  %llX\n", left_canary);
        fprintf(ostream, "RIGHT DATA CANARY PROTECT: %llX\n\n", rigth_canary);
    }
#endif /* ON_CANARY_PROTECT */
#ifdef ON_HASH_PROTECT
    fprintf(ostream, "STACK HASH:  " HASH_SPEC "\nDATA HASH:  " HASH_SPEC "\n",
                     stk->hash_data, stk->hash_stack);
#endif /* ON_HASH_PROTECTION */
    fprintf(ostream, "---------------------------------------------------------------------------------\n\n");
    return stk->error;
}

error_t stack_resize(my_stack_t* stk, mem_modify_t mode) {
    STACK_ASSERT_(stk);

    if (mode == EXPAND) {
        stk->capacity = stk->capacity * CAPACITY_COEFF;
    }
    else {
        stk->capacity = stk->capacity / CAPACITY_COEFF;
    }
#ifdef ON_CANARY_PROTECT
    void* new_data_ptr = realloc((char*) stk->data - sizeof(canary_t),
                                  stk->capacity * stk->elm_width + 2 * sizeof(canary_t));
    if (!new_data_ptr) {
        stk->error = MEMORY_REALLOCATION_ERROR;
        return MEMORY_REALLOCATION_ERROR;
    }

    memcpy(new_data_ptr, &DATA_CANARY, sizeof(canary_t));
    stk->data = (void*) ((char*) new_data_ptr + sizeof(canary_t));
    memcpy((char*) stk->data + stk->capacity * stk->elm_width, &DATA_CANARY, sizeof(canary_t));
#else
    void* new_data_ptr = realloc(stk->data, stk->capacity * stk->elm_width);
    if (!new_data_ptr) {
        stk->error = MEMORY_REALLOCATION_ERROR;
        return MEMORY_REALLOCATION_ERROR;
    }
    stk->data = new_data_ptr;
#endif /* ON_CANARY_PROTECT */
    return NO_ERRORS;
}

error_t stack_error(my_stack_t* stk) {
    assert(stk != nullptr);

#ifdef ON_CANARY_PROTECT
    if ((stk->left_canary ^ (uint64_t) stk) != LEFT_CANARY_MASK) {
        stk->error = LEFT_CANARY_PROTECT_FAILURE;
        return LEFT_CANARY_PROTECT_FAILURE;
    }

    if ((stk->rigth_canary ^ (uint64_t) stk) != RIGTH_CANARY_MASK) {
        stk->error = RIGTH_CANARY_PROTECT_FAILURE;
        return RIGTH_CANARY_PROTECT_FAILURE;
    }
#endif /* ON_CANARY_PROTECT */

    if (stk->error != NO_ERRORS) {
        return stk->error;
    }

    if (!stk->capacity) {
        stk->error = NULL_CAPACITY_ERROR;
        return NULL_CAPACITY_ERROR;
    }

    if (stk->capacity >= MAX_CAPACITY) {
        stk->error = CAPACITY_LIMIT_EXCEED_ERROR;
        return CAPACITY_LIMIT_EXCEED_ERROR;
    }

    if (stk->capacity < stk->size) {
        stk->error = INVALID_SIZE_ERROR;
        return INVALID_SIZE_ERROR;
    }

    if (!stk->data) {
        stk->error = DATA_INVALID_POINTER_ERROR;
        return DATA_INVALID_POINTER_ERROR;
    }

    if (!stk->elm_width) {
        stk->error = NULL_ELEMENT_WIDTH_ERROR;
        return NULL_ELEMENT_WIDTH_ERROR;
    }

#ifdef ON_CANARY_PROTECT
    if (memcmp((canary_t*) stk->data - 1, &DATA_CANARY, sizeof(canary_t)) ||
        memcmp((char*) stk->data + stk->capacity * stk->elm_width, &DATA_CANARY, sizeof(canary_t))) {
        stk->error = DATA_CANARY_PROTECT_FAILURE;
        return DATA_CANARY_PROTECT_FAILURE;
    }
#endif /* ON_CANARY_PROTECT */

#ifdef ON_HASH_PROTECT
    hash_t hash_data = 0;
    hash_t hash_stack = 0;
    stack_hash(stk, &hash_data, &hash_stack);
    if (hash_data != stk->hash_data || hash_stack != stk->hash_stack) {
        stk->error = HASH_PROTECTION_FAILED;
        return HASH_PROTECTION_FAILED;
    }
#endif /* ON_HASH_PROTECT */

    stk->error = NO_ERRORS;
    return NO_ERRORS;
}

#ifdef ON_HASH_PROTECT
void set_stack_hash(my_stack_t* stk) {
    assert(stk != nullptr);

    hash_t new_hash_data = 0;
    hash_t new_hash_stack = 0;

    stack_hash(stk, &new_hash_data, &new_hash_stack);

    stk->hash_data = new_hash_data;
    stk->hash_stack = new_hash_stack;
}

hash_t stack_hash(my_stack_t* stk, hash_t* new_hash_data, hash_t* new_hash_stack) {
    assert(stk != nullptr);
    assert(new_hash_data != nullptr);
    assert(new_hash_stack != nullptr);

    void* data_ptr = stk->data;
    stk->data = nullptr;
    hash_t hash_stack = stk->hash_stack;
    stk->hash_stack = 0;
    hash_t hash_data = stk->hash_data;
    stk->hash_data = 0;

    *new_hash_stack = stack_hash_counter(stk, sizeof(stk));

    if (data_ptr == nullptr) {
        *new_hash_data = 0;
    }
    else {
        *new_hash_data  = stack_hash_counter(data_ptr, stk->capacity);
    }

    stk->hash_stack = hash_stack;
    stk->hash_data = hash_data;
    stk->data = data_ptr;
    return *new_hash_stack;
}
hash_t stack_hash_counter(const void* buffer, size_t buffer_bytes_amount) {
    const char* new_buffer = (const char*) buffer;
    hash_t hash = 5381;
    for (size_t i = 0; i < buffer_bytes_amount; i++) {
        hash = hash * 31 ^ (hash_t) *new_buffer++; /* hash * 33 + c */
    }
    return hash;
}
#endif /* ON_HASH_PROTECT */

static void print_time(FILE *ostream) {
    assert(ostream != nullptr);

    time_t mytime = time(NULL);
    struct tm *time = localtime(&mytime);

    fprintf(ostream, "%02d.%02d.%d %02d:%02d:%02d ",
            time->tm_mday, time->tm_mon + 1, time->tm_year + 1900,
            time->tm_hour, time->tm_min,     time->tm_sec);
}

static const char* stack_error_message(error_t error) {
    switch(error) {
        CASE_(NO_ERRORS);
        CASE_(MEMORY_ALLOCATION_ERROR);
        CASE_(MEMORY_REALLOCATION_ERROR);
        CASE_(ELEMENT_WIDTH_ERROR);
        CASE_(NO_ELEMNTS_TO_POP_ERROR);
        CASE_(NULL_CAPACITY_ERROR);
        CASE_(INVALID_SIZE_ERROR);
        CASE_(DATA_INVALID_POINTER_ERROR);
        CASE_(NULL_ELEMENT_WIDTH_ERROR);
        CASE_(STACK_ALREADY_INITIALIZED_ERROR);
        CASE_(CAPACITY_LIMIT_EXCEED_ERROR);
#ifdef ON_CANARY_PROTECT
        CASE_(LEFT_CANARY_PROTECT_FAILURE);
        CASE_(RIGTH_CANARY_PROTECT_FAILURE);
        CASE_(DATA_CANARY_PROTECT_FAILURE);
#endif /* ON_CANARY_PROTECT */
#ifdef ON_HASH_PROTECT
        CASE_(HASH_PROTECTION_FAILED);
#endif /* ON_HASH_PROTECT */
        default:
            break;
    }
}
