#include "disassembler.h"

#define DEF_CMD(name, num, has_arg, ...)   \
case num:                                  \
    if(has_arg)                            \
    {                                      \
        fprintf(dest, "%s ", #name);       \
        fprint_arg(num, &ip, code, dest);  \
        ip++;                              \
    }                                      \
    else                                   \
    {                                      \
        fprintf(dest, "%s\n", #name);      \
    }                                      \
    break;                                 \

#define PRINT_REG(fmt, reg) fprintf(dest, fmt, #reg)

#define CASE_PRINT_REGS(fmt)               \
case REG_RAX:                              \
    PRINT_REG(fmt, rax);                   \
    break;                                 \
case REG_RBX:                              \
    PRINT_REG(fmt, rbx);                   \
    break;                                 \
case REG_RCX:                              \
    PRINT_REG(fmt, rcx);                   \
    break;                                 \
case REG_RDX:                              \
    PRINT_REG(fmt, rdx);                   \
    break;

void disasm(double *code, size_t max_ip, FILE *dest)
{
    assert(code);
    assert(max_ip);
    assert(dest);

    size_t ip = 0; //instruction pointer

    for(; ip < max_ip; ip++)
    {
        switch(*((int*)(code + ip)))
        {
            #include "../commands.h"
            default:
                printf("error: command number %d not found\n", code[ip]);
                printf("ip = %d\n", ip);
        }
    }

}

void fprint_arg(size_t num, size_t *ip_ptr, double *code, FILE *dest)
{
    assert(dest);
    assert(ip_ptr);

    size_t ip = *ip_ptr;

    switch(num)
    {
        case CMD_push:
            if(CHECK_MASK(N_REG) && CHECK_MASK(N_RAM))
            {
                (*ip_ptr)++;
                ip++;

                switch((int)code[ip])
                {
                    CASE_PRINT_REGS("[%s");
                }

                fprintf(dest, " + %d]\n", (int)code[ip + 1]);
            }
            else if(CHECK_MASK(N_IMMED))//immediate argument
            {
                fprintf(dest, "%.2lf\n", code[ip + 1]);
            }
            else if(CHECK_MASK(N_RAM))//RAM
            {
                fprintf(dest, "[%d]\n", (int)code[ip + 1]);
            }
            else if(CHECK_MASK(N_REG))//register
            {
                switch((int)code[ip + 1])
                {
                    CASE_PRINT_REGS("%s\n");
                }
            }
        break;

        case CMD_pop:
            if(CHECK_MASK(N_REG) && CHECK_MASK(N_RAM))
            {
                (*ip_ptr)++;
                ip++;

                switch((int)code[ip])
                {
                    CASE_PRINT_REGS("[%s");
                }

                fprintf(dest, " + %d]\n", (int)code[ip + 1]);
            }
            else if(CHECK_MASK(N_RAM))
            {
                fprintf(dest, "[%d]\n", (int)code[ip]);
            }
            else if(CHECK_MASK(N_REG))
            {
                switch((int)code[ip + 1])
                {
                    CASE_PRINT_REGS("%s\n");
                }
            }
            break;

        default: //for jumps and call
            fprintf(dest, "%d\n",(int)code[ip + 1]);
    }

    return;
}
