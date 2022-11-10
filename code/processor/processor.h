#ifndef PROCESSOR
#define PROCESSOR

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "../proc_constants.h"
#include "simple_stack.h"

#define EPSILON 1e-7
#define RAM_SIZE 100

//! @param[in] n1 First number.
//! @param[in] n2 Second number.
//! @return Returns 1 or 0.
//! @brief Returns 1 if |n1 - n2| <= EPSILON and 0 otherwise.
int are_doubles_equal(double n1, double n2);

//! @param[in] code Double array.
//! @param[in] max_ip Max instruction pointer.
void execute_code(double *code, size_t max_ip);

#endif
