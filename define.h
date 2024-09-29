#ifndef DEFINE_H
#define DEFINE_H

#define STATIC_ASSERT(COND,MSG)                           \
    do                                                    \
    {                                                     \
        typedef char static_assertion_##MSG[(COND)?1:-1]; \
        static_assertion_##MSG x = {};                    \
        (void) x;                                         \
    } while (0)

#define _POS __FILE__, __LINE__, __FUNCTION__

#define STACK_DUMP_(stk)      \
        stack_dump(stk, _POS)

#define STACK_ASSERT_(stk)      \
        stack_assert(stk, _POS)

#define STACK_CTOR_(stk, size, capacity)                         \
        stack_ctor(stk, size, capacity ON_DEBUG(,{#stk, _POS}));

#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

#define CASE_(stk)   \
    case stk:        \
        return #stk; \
        break

#endif /* DEFINE_H */
