#include "processor.h"
#include "../dsl.h"

#define DEF_CMD(name, num, has_arg, ...) \
case num:                                \
    __VA_ARGS__                          \
    break;                               \

int are_doubles_equal(double n1, double n2)
{
    return (fabs(n1 - n2) <= EPSILON);
}

void execute_code(double *code, size_t max_ip)
{
    assert(code);
    assert(max_ip);

    size_t ip = 0; //instruction pointer

    my_stk stk = {};
    STK_CTOR(&stk, DEFAULT_STK_CAP, stdout);

    my_stk ret_address_stk = {};
    STK_CTOR(&ret_address_stk, DEFAULT_STK_CAP, stdout);

    double RAM[RAM_SIZE + VID_WIDTH*VID_HEIGHT + 1] = {0};
    double regs[5] = {0};
    double arg = NAN;

    for(; ip < max_ip; ip++)
    {
        arg = NAN;
        switch(*((int*)(code + ip)))
        {
            #include "../commands.h"
            default:
                printf("error: command number %d not found\n", code[ip]);
                printf("ip = %d\n", ip);
        }
    }

    stk_dtor(&stk);
    stk_dtor(&ret_address_stk);

    return;
}
