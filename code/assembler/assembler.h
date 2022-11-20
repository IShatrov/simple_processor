#ifndef ASSEMBLER
#define ASSEMBLER

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include <string.h>

#include "../proc_constants.h"

#define MAX_CMD_LEN 101
#define MAX_LBL_N 20

struct label
{
    ssize_t address;
    char *name;
};

typedef struct label s_lbl;

//! @param[in] stream File to examine.
//! @return Returns amount of characters in stream.
//! @brief Gets amount of characters in stream.
size_t get_file_size(FILE *stream);

//! @param[in] stream File to read from.
//! @return Returns pointer to char array.
//! @brief Reads text from stream and stores it as char array. The array is null-terminated.
char* read_code(FILE *stream);

//! @param[in] chr Character to count.
//! @param[in] str Pointer to string to count at.
//! @return Returns amount of occurrences of chr in str.
size_t count_chr_in_str(char chr, const char *str);

//! @param[in] file File to read code from.
//! @return Returns pointer to array of doubles.
//! @brief Converts code from text to "machine" code.
double* parse_code(FILE *stream, s_lbl *labels, char *text, file_head *head);

//! @param[in] str Pointer to char array.
//! @param[in] cmd Pointer to char array.
//! @return Returns 0 if str equals cmd and non-zero value otherwise.
//! Compares char arrays lexicographically. Case insensitive. Stops if detects any non-letter character.
int cmd_cmp(const char *str, const char *cmd);

//! @param[in] code Pointer to "machine" code array.
//! @param[in] stream File to write to.
//! @brief Writes listing..
void create_listing(const double *code, FILE *stream, file_head *head);

//! @param[in] num Number of command.
//! @param[in] ip Instruction pointer.
//! @param[in] code Double array.
//! @param[in] dest Destination file.
//! @brief Prints command argument.
void fprint_arg(size_t num, ssize_t *ip_ptr, const double *code, FILE *dest);

//! @param[in] code Pointer to "machine" code array.
//! @param[in] stream File to write to.
//! @brief Writes "machine" code from code to stream as its binary representation.
void output_bin(const double *code, FILE *stream, file_head *head);

//! @param[in] labels Labels array.
//! @param[in] name Name of label to find.
//! @return Returns index of label if it was found and -1 otherwise.
//! @brief Finds label in labels array.
int find_label(s_lbl *labels, const char* name);

//! @param[in] num Number of command.
//! @param[in] text Pointer to code text.
//! @param[in] labels Labels array.
//! @param[in] machine_code Double array.
//! @param[in] cells_filled Amount of cells filled in code array.
//! @brief Gets argument for some commands
void get_arg(int num, char *text, s_lbl *labels, double *machine_code, ssize_t *cells_filled);

#endif
