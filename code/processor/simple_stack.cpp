#include "simple_stack.h"

void stk_ctor(my_stk *stk, ssize_t capacity, FILE *log, struct var_info info)
{
    assert(capacity > 0);
    assert(stk != NULL);

    #ifdef CANARY
    stk->data_cnry_l = (cnry*)calloc(capacity + 2, sizeof(stk_elem));
    assert(stk->data_cnry_l != NULL);
    stk->data = (stk_elem*)(stk->data_cnry_l) + 1;
    stk->data_cnry_r = (stk->data_cnry_l) + capacity + 1;

    *(stk->data_cnry_l) = (cnry)(stk->data);
    *(stk->data_cnry_r) = (cnry)(stk->data);
    #else
    stk->data = (stk_elem*)calloc(capacity, sizeof(stk_elem));
    assert(stk->data != NULL);
    #endif

    stk->info = info;

    stk->capacity = capacity;
    stk->size = 0;

    stk->log = log;

    #ifdef CANARY
    stk->stk_cnry_l = (cnry)stk;
    stk->stk_cnry_r = (cnry)stk;
    #endif

    #ifdef HASH
    stk->hash_num = my_hash(stk, sizeof(*stk) - sizeof(unsigned long long int));
    #endif

    #ifdef STK_OK
    if(!STK_OK(stk)) return;
    #endif

    return;
}

void stk_recalloc(my_stk *stk, ssize_t new_cap)
{
    assert(new_cap > 0);

    #ifdef STK_OK
    if(!STK_OK(stk)) return;
    #endif

    #ifdef CANARY
    stk->data_cnry_l = (cnry*)realloc(stk->data_cnry_l, (new_cap + 2)*sizeof(stk_elem));
    assert(stk->data_cnry_l != NULL);
    stk->data = (stk_elem*)(stk->data_cnry_l) + 1;
    stk->data_cnry_r = (cnry*)(stk->data_cnry_l) + new_cap + 1;

    *(stk->data_cnry_l) = (cnry)(stk->data);
    *(stk->data_cnry_r) = (cnry)(stk->data);
    #else
    stk->data = (stk_elem*)realloc(stk->data, new_cap*sizeof(stk_elem));
    assert(stk->data != NULL);
    #endif

    #ifdef HASH
    stk->hash_num = my_hash(stk, sizeof(*stk) - sizeof(unsigned long long int));
    #endif

    #ifdef STK_OK
    if(!STK_OK(stk)) return;
    #endif

    if(new_cap > stk->capacity)
    {
        stk_elem *ptr = stk->data + stk->size;

        for(ssize_t i = 0; i < new_cap - (stk->capacity); i++)
        {
            *ptr = 0;
            ptr++;
        }
    }

    stk->capacity = new_cap;
    stk->size = (new_cap > stk->size)?stk->size:new_cap;

    return;
}

void stk_push(my_stk *stk, stk_elem value)
{
    #ifdef STK_OK
    if(!STK_OK(stk)) return;
    #endif

    if(stk->size >= stk->capacity)
    {
        stk_recalloc(stk, 2*stk->capacity);
    }

    (stk->data)[(stk->size)++] = value;

    #ifdef HASH
    stk->hash_num = my_hash(stk, sizeof(*stk) - sizeof(unsigned long long int));
    #endif

    return;
}

stk_elem stk_pop(my_stk *stk)
{
    #ifdef STK_OK
    if(!STK_OK(stk)) return NAN;
    #endif

    stk_elem ans = stk->data[--(stk->size)];
    stk->data[stk->size] = 0;

    if(3*(stk->size) < stk->capacity && stk->capacity > MIN_REC_CAPACITY)
    {
        stk_recalloc(stk, stk->capacity/3);
    }

    #ifdef HASH
    stk->hash_num = my_hash(stk, sizeof(*stk) - sizeof(unsigned long long int));
    #endif

    #ifdef STK_OK
    if(!STK_OK(stk)) return NAN;
    #endif

    return ans;
}

void stk_dtor(my_stk *stk)
{
    #ifdef STK_OK
    if(!STK_OK(stk)) return;
    #endif

    for(ssize_t i = 0; i < stk->capacity; i++) stk->data[i] = NAN;

    stk->size = -1;
    stk->capacity = -1;

    #ifdef CANARY
    free(stk->data_cnry_l);
    #else
    free(stk->data);
    #endif

    stk->data = NULL;

    return;
}

unsigned long long int my_hash(const void *data, size_t size)
{
    assert(data != NULL);

    unsigned long long ans = 0;

    for(size_t i = 0; i < size; i++)
    {
        ans = ((ans << 5) + ans) + *((const char*)data);

        data = (const char*)data + 1;
    }

    return ans;
}
