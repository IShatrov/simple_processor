#include "assembler.h"
#include "../dsl.h"

#define SKIP_SPACES(text) while(*(text) == ' ' || *(text) == '\t') text++
#define SKIP_ALL(text) while(*(text) != '\n' && *(text) != '\0') text++
#define SKIP_SEMICOLON(text) if(*(text) == ';') SKIP_ALL(text)

#define DEF_CMD(name, num, has_arg, ...)                            \
else if(!cmd_cmp(cmd, #name))                                       \
{                                                                   \
    *((int*)(machine_code + cells_filled++)) = num;                 \
    text += strlen(#name);                                          \
                                                                    \
    if(has_arg)                                                     \
    {                                                               \
        get_arg(num, text, labels, machine_code, &cells_filled);    \
        cells_filled++;                                             \
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

#define REGS_TO_CODE(n_instr)                                    \
if(!cmd_cmp(cmd, "rax")) machine_code[n_instr] = REG_RAX;        \
else if(!cmd_cmp(cmd, "rbx")) machine_code[n_instr] = REG_RBX;   \
else if(!cmd_cmp(cmd, "rcx")) machine_code[n_instr] = REG_RCX;   \
else if(!cmd_cmp(cmd, "rdx")) machine_code[n_instr] = REG_RDX;   \
else printf("syntax error: reg %5s not found\n", cmd);           \

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

double* parse_code(FILE *stream, s_lbl *labels, char *text, file_head *head)
{
    assert(stream != NULL);
    assert(labels);
    assert(text);
    assert(head);

    static int n_pass = 0;

    size_t n_lines = 1 + count_chr_in_str('\n', text); //last line does not end with \n

    double *machine_code = (double*)calloc(2*n_lines, sizeof(double));
    assert(machine_code != NULL);

    head->sign = SIGNATURE;
    head->ver = VERSION;
    head->n_lines = 0;

    ssize_t cells_filled = 0;

    char cmd[MAX_CMD_LEN] = {0};
    size_t n_labels = 0;

    while(*(text) != '\0')
    {
        SKIP_SPACES(text);   //ignore empty lines
        if(*text == '\n')
        {
            text++;
            continue;
        }

        sscanf(text, "%s", cmd);

        char *index_of_colon = NULL;

        if((index_of_colon = strchr(cmd, ':')))
        {
            //printf("%s\n", cmd);

            int index = -1;
            if((index = find_label(labels, cmd)) >= 0)
            {
                labels[index].address = cells_filled;
            }
            else
            {
                labels[n_labels++] = {cells_filled, text};
            }

            SKIP_ALL(text);
            text++;

            *((int*)(machine_code + cells_filled++)) = CMD_cont;
        }

        #include "../commands.h"

        else
        {
            printf("error: command not found\n");

            break;
        }
    }

    head->n_lines = cells_filled;

    n_pass++;

    return machine_code;
}

#undef DEF_CMD

int cmd_cmp(const char *str, const char *cmd)
{
    assert(str != NULL);
    assert(cmd != NULL);

    while(tolower(*str) == tolower(*cmd))
    {
        str++;
        cmd++;

        if(((*str == '\0') + (*cmd == '\0') + (*str == ':') + (*cmd == ':')) >= 2)
        {
            str--;
            cmd--;
            break;
        }
    }

    return tolower(*cmd) - tolower(*str);
}

#define DEF_CMD(name, num, has_arg, ...)          \
case num:                                         \
    if(has_arg)                                   \
    {                                             \
        fprintf(stream, "%d %s ", ip, #name);     \
        fprint_arg(num, &ip, code, stream);       \
        ip++;                                     \
    }                                             \
    else                                          \
    {                                             \
        fprintf(stream, "%d %s\n", ip, #name);    \
    }                                             \
    break;

void create_listing(const double *code, FILE *stream, file_head *head)
{
    assert(code);
    assert(stream);
    assert(head);

    fprintf(stream, "%d ", head->sign);      //signature
    fprintf(stream, "%d ", head->ver);//version
    fprintf(stream, "%d\n", head->n_lines);//n of instructions

    ssize_t ip = 0;
    ssize_t max_ip = head->n_lines;

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

    return;
}

void fprint_arg(size_t num, ssize_t *ip_ptr, const double *code, FILE *dest)
{
    assert(dest);
    assert(ip_ptr);
    assert(code);

    ssize_t ip = *ip_ptr;

    switch(num)
    {
        case CMD_push:
            if(CHECK_MASK(N_REG) && CHECK_MASK(N_RAM))
            {
                (*ip_ptr)++;
                ip++;

                switch((int)code[ip])
                {
                    CASE_PRINT_REGS("[%s +");
                }

                fprintf(dest, " %d]\n", (int)code[ip + 1]);
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
                    CASE_PRINT_REGS("[%s +");
                }

                fprintf(dest, " %d]\n", (int)code[ip + 1]);
            }
            else if(CHECK_MASK(N_RAM))
            {
                fprintf(dest, "[%d]\n", (int)code[ip + 1]);
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

#undef DEF_CMD

void output_bin(const double *code, FILE *stream, file_head *head)
{
    assert(code);
    assert(stream);
    assert(head);

    fwrite(head, sizeof(file_head), 1, stream);

    fwrite(code, (head->n_lines)*sizeof(double), 1, stream);

    return;
}

int find_label(s_lbl *labels, const char* name)
{
    assert(labels);
    assert(name);

    for(int i = 0; i < MAX_LBL_N; i++)
    {
        //printf("comparing %.10s %.10s res: %d\n", name, labels[i].name, labels[i].name && !(cmd_cmp(name, labels[i].name)));

        if(labels[i].name && !(cmd_cmp(name, labels[i].name)))
        {
            return i;
        }
    }

    return -1;
}

void get_arg(int num, char *text, s_lbl *labels, double *machine_code, ssize_t *cells_filled)
{
    assert(text);
    assert(labels);
    assert(machine_code);
    assert(cells_filled);

    double arg = 0;
    char cmd[MAX_CMD_LEN] = {0};

    switch(num)
    {
        case CMD_push:
            arg = 0;
            if(sscanf(text, " [%s + %lf]", cmd, &arg) == 2)
            {
                WRITE_MASK(N_REG);
                WRITE_MASK(N_RAM);

                REGS_TO_CODE((*cells_filled)++);

                machine_code[*cells_filled] = arg;
            }
            else if(sscanf(text, "%lf", &arg))
            {
                WRITE_MASK(N_IMMED);

                machine_code[*cells_filled] = arg; //immediate argument
            }
            else if(sscanf(text, " [%lf]", &arg))
            {
                WRITE_MASK(N_RAM);//RAM

                machine_code[*cells_filled] = arg;
            }
            else if(sscanf(text, "%s", cmd))
            {
                WRITE_MASK(N_REG);//register

                REGS_TO_CODE(*cells_filled);
            }
            else
            {
                printf("syntax error: push argument not found\n");
            }
            break;

        case CMD_pop:
            if(sscanf(text, " [%s + %lf]", cmd, &arg) == 2)
            {
                WRITE_MASK(N_REG);
                WRITE_MASK(N_RAM);

                REGS_TO_CODE((*cells_filled)++);

                machine_code[*cells_filled] = arg;
            }
            else if(sscanf(text, " [%lf]", &arg))
            {
                WRITE_MASK(N_RAM);;//RAM
                machine_code[*cells_filled] = arg;
            }
            else if(sscanf(text, "%s", cmd))
            {
                WRITE_MASK(N_REG);//register

                REGS_TO_CODE(*cells_filled);
            }
            else
            {
                printf("syntax error: pop argument not found\n");
            }
            break;

        default: //for jumps and call
            if(sscanf(text, "%lf", &arg)) machine_code[*cells_filled] = arg;
            else
            {
                sscanf(text, "%s", cmd);

                int index = -1;

                //printf("searching for %.10s\n", cmd);

                if((index = find_label(labels, cmd)) >= 0)
                {
                    machine_code[*cells_filled] = labels[index].address;
                }
                else
                {
                    machine_code[*cells_filled] = -1;
                }

                //printf("%lf\n", machine_code[cells_filled]);
            }
    }

    return;
}
