#ifndef DEFINE_H
#define DEFINE_H

#define STATIC_ASSERT(COND,MSG)                           \
    do                                                    \
    {                                                     \
        typedef char static_assertion_##MSG[(COND)?1:-1]; \
        static_assertion_##MSG x_ = {};                   \
        (void) x_;                                        \
    } while (0)

#endif /* DEFINE_H */
