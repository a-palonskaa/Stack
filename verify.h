#ifndef VERIFY
#define VERIFY

#include "stack.h"

void set_stack_dump_ostream(FILE* ostream);
error_t stack_dump(const my_stack_t* stk, const char* file, size_t line, const char* func);
error_t stack_error(my_stack_t* stk);
error_t stack_assert(my_stack_t* stk, const char* file, size_t line, const char* func);

#endif /* VERIFY */
