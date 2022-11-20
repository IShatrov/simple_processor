#define PUSH(n) stk_push(&stk, n)

#define POP stk_pop(&stk)

#define DO_JUMP(condition)      \
    if(condition)               \
    {                           \
        ip = code[ip + 1] - 1;  \
    }                           \
    else ip++;

#define CHECK_MASK(mask) (*((int*)(code + ip) + 1) & (mask))

#define WRITE_MASK(mask) *((int*)(machine_code + *cells_filled - 1) + 1) |= (mask)
