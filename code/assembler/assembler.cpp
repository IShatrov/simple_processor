#include "assembler.h"

int main(int argc, const char *argv[])
{
    const char *source_filename = "code.txt";
    if (argc > 1) source_filename = argv[1];

    FILE *code = NULL;
    FILE *listing = NULL;
    FILE *bin_out = NULL;

    code = fopen(source_filename, "r");
    listing = fopen("listing.txt", "w");
    bin_out = fopen("out.bin", "wb");

    double *machine_code = NULL;

    if(code && listing && bin_out)
    {
        char *text = read_code(code);

        s_lbl *labels = (s_lbl*)calloc(MAX_LBL_N, sizeof(s_lbl));
        assert(labels);

        file_head head = {0, 0, 0};

        machine_code = parse_code(code, labels, text, &head); //two passes to fill labels array

        //for(int i = 0; i < MAX_LBL_N; i++) printf("%d: %.10s %d\n", i, labels[i].name, labels[i].address);

        machine_code = parse_code(code, labels, text, &head);

        //putchar('\n');
        //for(int i = 0; i < MAX_LBL_N; i++) printf("%d: %.10s %d\n", i, labels[i].name, labels[i].address);

        create_listing(machine_code, listing, &head);
        output_bin(machine_code, bin_out, &head);

        free(text);
    }

    free(machine_code);

    fclose(code);
    fclose(listing);
    fclose(bin_out);

    getchar();

    return 0;
}
