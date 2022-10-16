#include "assembler.h"

#define SKIP_SPACES(text) while(*(text) == ' ' || *(text) == '\t') text++
#define SKIP_ALL(text) while(*(text) != '\n' && *(text) != '\0') text++
#define SKIP_SEMICOLON(text) if(*(text) == ';') SKIP_ALL(text)

#define DEF_CMD(name, num, has_arg, ...)                            \
else if(!cmd_cmp(cmd, #name))                                       \
{                                                                   \
    byte_cpy_num(&machine_code[cells_filled++], num, sizeof(int));  \
    text += strlen(#name);                                          \
                                                                    \
    if(has_arg)                                                     \
    {                                                               \
        get_arg(num, text, labels, machine_code, cells_filled++);   \
                                                                    \
        SKIP_ALL(text);                                             \
        text++;                                                     \
    }                                                               \
    else                                                            \
    {                                                               \
        SKIP_SPACES(text);                                          \
        SKIP_SEMICOLON(text);                                       \
        text++;                                                     \
    }                                                               \
}                                                                   \

size_t get_file_size(FILE *stream)
{
    assert(stream != NULL);

    size_t ans = 0;

    assert(!fseek(stream, 0, SEEK_END));   //fseek returns zero upon success and non-zero otherwise
    ans = ftell(stream);
    assert(!ferror(stream));

    assert(!fseek(stream, 0, SEEK_SET));

    return ans;
}

char* read_code(FILE *stream)
{
    assert(stream != NULL);

    size_t len = get_file_size(stream);
    assert(len != 0);

    char *text = (char*)calloc(len + 1, sizeof(char));
    assert(text != NULL);

    fread(text, sizeof(char), len, stream);
    assert(!ferror(stream));

    text[len] = '\0';

    return text;
}

size_t count_chr_in_str(char chr, const char *str)
{
    assert(str != NULL);

    size_t ans = 0, chars_checked = 0;

    while (*(str + chars_checked) != '\0')
    {
        if (*(str + chars_checked) == chr) ++ans;

        ++chars_checked;
    }

    return ans;
}

double* parse_code(FILE *stream, s_lbl *labels, char *text)
{
    assert(stream != NULL);

    static int n_pass = 0;

    size_t n_lines = 1 + count_chr_in_str('\n', text); //last line does not end with \n

    double *machine_code = (double*)calloc(2*n_lines + 3, sizeof(double));
    assert(machine_code != NULL);            //there will be no more than 2*n_lines words in source code,
                                             //and 3 more cells are required for signature, version and n_lines
    byte_cpy_num(&machine_code[0], SIGNATURE, sizeof(int));
    byte_cpy_num(&machine_code[1], VERSION, sizeof(int));
    byte_cpy_num(&machine_code[2], 2, sizeof(int));

    ssize_t cells_filled = 3;

    char cmd[MAX_CMD_LEN] = {0};
    size_t n_labels = 0;

    while(*(text) != '\0')
    {
        sscanf(text, "%s", cmd);

        if(strchr(cmd, ':'))
        {
            int index = -1;
            if((index = find_label(labels, cmd)) >= 0)
            {
                labels[index].address = cells_filled - 3;
            }
            else
            {
                labels[n_labels++] = {cells_filled - 3, text};
            }

            SKIP_ALL(text);
            text++;

            byte_cpy_num(&machine_code[cells_filled++], CMD_CONT, sizeof(int));
        }

        #include "../commands.h"

        else
        {
            printf("error: command not found\n");

            break;
        }
    }

    free(text);

    byte_cpy_num(&machine_code[2], cells_filled, sizeof(int));

    n_pass++;

    return machine_code;
}

int cmd_cmp(const char *str, const char *cmd)
{
    assert(str != NULL);
    assert(cmd != NULL);

    while(tolower(*str) == tolower(*cmd))
    {
        if(!isalpha(*str) && !isalpha(*cmd)) break;

        str++;
        cmd++;
    }

    if(!isalpha(*str) && !isalpha(*cmd))
    {
        str--;
        cmd--;
    }

    return tolower(*cmd) - tolower(*str);
}

void create_listing(const double *code, FILE *stream)
{
    assert(code);
    assert(stream);

    fprintf(stream, "%d ", *((const int*)code));      //signature
    fprintf(stream, "%d ", *((const int*)(code + 1)));//version
    fprintf(stream, "%d\n", *((const int*)(code + 2)));//n of instructions

    for(int i = 3; i < *((const int*)(code + 2)); i++)
    {
        if(*((const int *)(code + i)) == CMD_POP || *((const int *)(code + i)) == CMD_PUSH ||
          *((const int *)(code + i)) >= CMD_JMP)
        {
            fprintf(stream, "%4d\t%d\t%.3lf\n", i - 3, *((const int*)(code + i)), *(code + i + 1));
            i++;
        }
        else
        {
            fprintf(stream, "%4d\t%d\n", i - 3, *((const int *)(code + i)));
        }
    }

    return;
}

void output_bin(const double *code, FILE *stream)
{
    assert(code);
    assert(stream);

    for(int i = 0; i < *((const int*)(code) + 4); i++) fwrite(&code[i], sizeof(double), 1, stream);

    return;
}

void byte_cpy_num(void *dest, int src, size_t size) // memcpy
{
    assert(dest != NULL);
    assert(size != 0);

    void *src_ptr = &src;

    while(size >= sizeof(double))
    {
        *((double*)dest) = *((double*)src_ptr);

        dest = ((double*)dest) + 1;
        src_ptr = ((double*)src_ptr) + 1;

        size -= sizeof(double);
    }

    while(size >= sizeof(int))
    {
        *((int*)dest) = *((int*)src_ptr);

        dest = ((int*)dest) + 1;
        src_ptr = ((int*)src_ptr) + 1;

        size -= sizeof(int);
    }

    while(size > 0)
    {
        *((char*)dest) = *((char*)src_ptr);

        dest = ((char*)dest) + 1;
        src_ptr = ((char*)src_ptr) + 1;

        size -= 1;
    }

    return;
}

int find_label(s_lbl *labels, const char* name)
{
    assert(labels);
    assert(name);

    for(int i = 0; i < MAX_LBL_N; i++)
    {
        if(labels[i].name && !(cmd_cmp(name, labels[i].name)))
        {
            return i;
        }
    }

    return -1;
}

void get_arg(int num, char *text, s_lbl *labels, double *machine_code, int cells_filled)
{
    assert(text);
    assert(labels);
    assert(machine_code);

    double arg = 0;
    char cmd[MAX_CMD_LEN] = {0};

    switch(num)
    {
        case CMD_PUSH:
            arg = 0;
            if(sscanf(text, "%lf", &arg))
            {                                     //cant use bit masks because machine_code is array of double
                byte_cpy_num((char*)(machine_code + cells_filled - 1) + sizeof(int), N_IMMED, sizeof(int));
                machine_code[cells_filled] = arg; //immediate argument
            }
            else if(sscanf(text, " [%lf]", &arg))
            {
                byte_cpy_num((char*)(machine_code + cells_filled - 1) + sizeof(int), N_RAM, sizeof(int));
                machine_code[cells_filled] = arg; //RAM
            }
            else if(sscanf(text, "%s", cmd))
            {
                byte_cpy_num((char*)(machine_code + cells_filled - 1) + sizeof(int), N_REG, sizeof(int));
                                                  //register
                if(!cmd_cmp(cmd, "rax")) machine_code[cells_filled] = REG_RAX;
                else if(!cmd_cmp(cmd, "rbx")) machine_code[cells_filled] = REG_RBX; // macros
                else if(!cmd_cmp(cmd, "rcx")) machine_code[cells_filled] = REG_RCX;
                else if(!cmd_cmp(cmd, "rdx")) machine_code[cells_filled] = REG_RDX;
                else printf("syntax error: reg %5s not found\n", cmd);
            }
            else
            {
                printf("syntax error: push argument not found\n");
            }
            break;

        case CMD_POP:
            if(sscanf(text, " [%lf]", &arg))
            {
                byte_cpy_num((char*)(machine_code + cells_filled - 1) + sizeof(int), N_RAM, sizeof(int));//RAM
                machine_code[cells_filled] = arg;
            }
            else if(sscanf(text, "%s", cmd))
            {
                byte_cpy_num((char*)(machine_code + cells_filled - 1) + sizeof(int), N_REG, sizeof(int));//register

                if(!cmd_cmp(cmd, "rax")) machine_code[cells_filled] = REG_RAX;
                else if(!cmd_cmp(cmd, "rbx")) machine_code[cells_filled] = REG_RBX;
                else if(!cmd_cmp(cmd, "rcx")) machine_code[cells_filled] = REG_RCX;
                else if(!cmd_cmp(cmd, "rdx")) machine_code[cells_filled] = REG_RDX;
                else printf("syntax error: reg %5s not found\n", cmd);
            }
            else
            {
                printf("syntax error: pop argument not found\n");
            }
            break;

        default: //for jumps and call
            if(sscanf(text, "%lf", &arg)) machine_code[cells_filled] = arg;
            else
            {
                sscanf(text, "%s", cmd);

                int index = -1;
                if((index = find_label(labels, cmd)) >= 0)
                {
                    machine_code[cells_filled] = labels[index].address;
                }
                else
                {
                    machine_code[cells_filled] = -1;
                }
            }
    }

    return;
}
