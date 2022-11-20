#include "processor.h"

int main(void)
{
    int is_error = 0;
    FILE *code_file = NULL;

    if(!(code_file = fopen("out.bin", "rb")))
    {
        printf("code file not found\n");
        is_error = 1;
    }

    file_head head = {0, 0, 0};
    fread(&head, sizeof(file_head), 1, code_file);

    if(head.sign != SIGNATURE)
    {
        printf("invalid signature %x\n", head.sign);
        is_error = 1;
    }
    if(head.ver != VERSION)
    {
        printf("invalid version %d\n", head.ver);
        is_error = 1;
    }

    if(!is_error)
    {
        size_t max_ip = head.n_lines;

        double *code = (double*)calloc((max_ip), sizeof(double));
        assert(code);

        fread(code, sizeof(double)*(max_ip), 1, code_file);

        execute_code(code, max_ip);

        free(code);
    }


    fclose(code_file);

    getchar();
    getchar();

    return 0;
}
