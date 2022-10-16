#include "disassembler.h"

int main(void)
{
    FILE *bin = NULL;
    FILE *dest = NULL;

    int is_error = 0;

    if(!(bin = fopen("out.bin", "rb")))
    {
        printf("bin file not found\n");
        is_error = 1;
    }

    if(!(dest = fopen("code.txt", "w")))
    {
        printf("dest file not found\n");
        is_error = 1;
    }

    double header[3] = {0};
    fread(header, 3*sizeof(double), 1, bin);

    if(*((int*)header) != SIGNATURE)
    {
        printf("invalid signature %x\n", *((int*)header));
        is_error = 1;
    }
    if(*((int*)header + 2) != VERSION)
    {
        printf("invalid version %d\n", *((int*)(header) + 2));
        is_error = 1;
    }

    if(!is_error)
    {
        size_t max_ip = (size_t)(*((int*)(header) + 4));

        double *code = (double*)calloc((max_ip), sizeof(double));
        assert(code);

        fread(code, sizeof(double)*(max_ip), 1, bin);

        disasm(code, max_ip, dest);

        free(code);
    }

    fclose(bin);
    fclose(dest);
    return 0;
}
