#ifndef SIMPLE_STACK_H
#define SIMPLE_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define STK_OK(stk) stk_assert(stk, __LINE__, __FILE__, __PRETTY_FUNCTION__)

#define GET_VAR_INFO(var) {#var, __PRETTY_FUNCTION__, __FILE__, __LINE__}

#define STK_CTOR(stk, capacity, log) stk_ctor(stk, capacity, log, GET_VAR_INFO(stk))

#define CANARY

#define HASH

enum err_codes
{
    NULL_STK_PTR = 1,
    NULL_DATA_PTR = 2,
    BAD_CAPACITY = 4,
    BAD_SIZE = 8,
    CAPACITY_LESS_SIZE = 16,

    #ifdef CANARY
    CANARY_DEAD = 32,
    #endif

    #ifdef HASH
    WRONG_HASH = 64,
    #endif
};

const ssize_t MIN_REC_CAPACITY = 30;

typedef double stk_elem;

#ifdef CANARY
typedef unsigned long long int cnry;
#endif

struct var_info
{
    const char *var_name;
    const char *func_name;
    const char *file_name;
    ssize_t line;
};

struct simple_stk
{
    #ifdef CANARY
    cnry stk_cnry_l;
    #endif

    #ifdef CANARY
    cnry *data_cnry_l;
    #endif

    stk_elem *data;

    #ifdef CANARY
    cnry *data_cnry_r;
    #endif

    ssize_t size;
    ssize_t capacity;
    FILE *log;

    struct var_info info;

    #ifdef CANARY
    cnry stk_cnry_r;
    #endif

    #ifdef HASH
    unsigned long long int hash_num;
    #endif
};

typedef struct simple_stk my_stk;

//! @param[in] stk Pointer to stack.
//! @param[in] capacity Desired initial stack capacity.
//! @param[in] log Pointer to stream to print logs to. Pass NULL to not print logs.
//! @param[in] info Struct containing information about stk variable.
//! @brief Constructs stack.
void stk_ctor(my_stk *stk, ssize_t capacity, FILE *log, struct var_info info);

//! @param[in] stk Pointer to stack.
//! @param[in] new_cap Desired capacity,
//! @brief Reallocates stack. If new capacity is increased fills empty space with 0.
void stk_recalloc(my_stk *stk, ssize_t new_cap);

//! @param[in] stk Pointer to stack.
//! @param[in] value Value  to push.
//! @brief Pushes value to stk. Increases stk capacity with stk_recalloc() if required.
void stk_push(my_stk *stk, stk_elem value);

//! @param[in] stk Pointer to stack.
//! @return Returns popped item.
//! @brief Pops item from stk. Decreases stk capacity with stk_recalloc if stk size becomes three times smaller than stk capacity,
stk_elem stk_pop(my_stk *stk);

//! @param[in] stk Pointer to stack.
//! @brief Destroys stk by filling all data and sizes with poison and freeing all memory.
void stk_dtor(my_stk *stk);

//! @param[in] log File to print to.
//! @param[in] stk Pointer to stack.
//! @param[in] line Number of line stk_dump() was called at.
//! @param[in] file File stk_dump was called in.
//! @param[in] func Function stk_dump was called in.
//! @param[in] err_code Error code.
//! @brief Prints information about stk in log.
void stk_dump(FILE *log, my_stk *stk, int line, const char *file, const char *func, int err_code);

//! @param[in] stk Pointer to stack.
//! @param[in] line Number of line stk_dump() was called at.
//! @param[in] file File stk_dump was called in.
//! @param[in] func Function stk_dump was called in.
//! @return Returns 1 if no errors were detected and 0 otherwise.
//! @brief Checks stk for errors and calls stk_dump() if errors were detected.
char stk_assert(my_stk *stk, int line, const char *file, const char *func);

//! @param[in] data Pointer to array to calculate hash for.
//! @param[in] size Size of the array.
//! @return Returns hash of the array.
//! @brief Calculates hash.
unsigned long long int my_hash(const void *data, size_t size);

#endif
