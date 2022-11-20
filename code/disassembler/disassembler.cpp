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

    file_head head = {0, 0, 0};
    fread(&head, sizeof(file_head), 1, bin);

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

        fread(code, sizeof(double)*(max_ip), 1, bin);

        disasm(code, max_ip, dest);

        free(code);
    }

    fclose(bin);
    fclose(dest);
    return 0;
}
