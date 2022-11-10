#include "processor.h"

int main(void)
{
    int is_error = 0;
    FILE *code_file = NULL;

    if(!(code_file = fopen("out.bin", "rb"))) //to-odo: command kine argumentts  and default name in header
    {
        printf("code file not found\n");
        is_error = 1;
    }

    double header[3] = {0};
    fread(header, 3*sizeof(double), 1, code_file);

    if(*((int*)header) != SIGNATURE)
    {
        printf("invalid signature %x\n", *((int*)header));
        is_error = 1;
    }
    if(*((int*)header + 2) != VERSION)//to-do: make struct
    {
        printf("invalid version %d\n", *((int*)(header) + 2));
        is_error = 1;
    }

    if(!is_error)
    {
        size_t max_ip = (size_t)(*((int*)(header) + 4));

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
