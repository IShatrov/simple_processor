#ifndef DISASSEMBLER
#define DISASSEMBLER

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../proc_constants.h"

#define DEF_CMD(name, num, has_arg, ...) \
CMD_##name = num,                        \

enum commands
{
    #include "../commands.h"
};

#undef DEF_CMD

//! @param[in] code Double array.
//! @param[in] max_ip Max instruction pointer.
//! @param[in] dest Destination file.
//! @brief Disassembles code and writes it to dest.
void disasm(double *code, size_t max_ip, FILE *dest);

//! @param[in] num Number of command.
//! @param[in] ip Instruction pointer.
//! @param[in] code Double array.
//! @param[in] dest Destination file.
//! @brief Prints command argument.
void fprint_arg(size_t num, size_t ip, double *code, FILE *dest);

#endif
