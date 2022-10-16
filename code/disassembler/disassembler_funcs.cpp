#include "disassembler.h"

#define DEF_CMD(name, num, has_arg, ...)   \
case num:                                  \
    if(has_arg)                            \
    {                                      \
        fprintf(dest, "%s ", #name);       \
        fprint_arg(num, ip++, code, dest); \
    }                                      \
    else                                   \
    {                                      \
        fprintf(dest, "%s\n", #name);      \
    }                                      \
    break;                                 \

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

void fprint_arg(size_t num, size_t ip, double *code, FILE *dest)
{
    assert(dest);

    switch(num)
    {
        case CMD_PUSH:
            if(*((int*)(code + ip) + 1) == N_IMMED)//immediate argument
            {
                fprintf(dest, "%.2lf\n", code[++ip]);
            }
            else if(*((int*)(code + ip) + 1) == N_RAM)//RAM
            {
                fprintf(dest, "[%d]\n", (int)code[++ip]);
            }
            else if(*((int*)(code + ip) + 1) == N_REG)//register
            {
                switch((int)code[++ip])
                {
                    case REG_RAX:
                        fprintf(dest, "rax\n");
                        break;
                    case REG_RBX:
                        fprintf(dest, "rbx\n");
                        break;
                    case REG_RCX:
                        fprintf(dest, "rcx\n");
                        break;
                    case REG_RDX:
                        fprintf(dest, "rdx\n");
                        break;
                }
            }
        break;

        case CMD_POP:
            if(*((int*)(code + ip) + 1) == N_RAM)
            {
                fprintf(dest, "[%d]\n", (int)code[++ip]);
            }
            else if(*((int*)(code + ip) + 1) == N_REG)
            {
                switch((int)code[++ip])
                {
                    case REG_RAX:
                        fprintf(dest, "rax\n");
                        break;
                    case REG_RBX:
                        fprintf(dest, "rbx\n");
                        break;
                    case REG_RCX:
                        fprintf(dest, "rcx\n");
                        break;
                    case REG_RDX:
                        fprintf(dest, "rdx\n");
                        break;
                }
            }
            break;

        default: //for jumps and call
            fprintf(dest, "%d\n",(int)code[++ip]);
    }

    return;
}
