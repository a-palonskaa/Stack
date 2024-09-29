#include <stdio.h>

#include "stack.h"
#include "pop_push.h"
#include "verify.h"
#include "define.h"

#ifdef ON_HASH_PROTECT
    #include "hash.h"
#endif /* ON_HASH_PROTECT */

int main() {
    const int BASE_CAPACITY = 2;

    FILE* ostream = fopen("logger.txt", "w");
    if (!ostream) {
        ostream = stdout;
    }

    set_stack_dump_ostream(ostream);

    my_stack_t stk = {};
    STACK_CTOR_(&stk, 10, BASE_CAPACITY, print_10bytes);

    int el = 0;
    for (size_t i = 0; i < 20; i++) {
        el = (int) i;
        stack_push(&stk, &el);
    }

    for (size_t i = 0; i < 30; i++) {
        if (stack_pop(&stk, &el) == NO_ERRORS) {
            printf("POPPED ELEMENT IS %d\n", el);
        }
    }
    stack_dtor(&stk);

    if (fclose(ostream) == EOF) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
