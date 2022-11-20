#ifndef DISASSEMBLER
#define DISASSEMBLER

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../dsl.h"
#include "../proc_constants.h"

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
void fprint_arg(size_t num, size_t *ip_ptr, double *code, FILE *dest);

#endif
