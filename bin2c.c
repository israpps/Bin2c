/**
 * This is bin2c program, which allows you to convert binary file to
 * C language array, for use as embedded resource, for instance you can
 * embed graphics or audio file directly into your program.
 * This is public domain software, use it on your own risk.
 * Contact Serge Fukanchik at fuxx@mail.ru  if you have any questions.
 *
 * Some modifications were made by Gwilym Kuiper (kuiper.gwilym@gmail.com)
 * I have decided not to change the licence.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    char *buf;
    char *ident;
    int write_size_as_macro = 0, write_guard = 0, add_hex_offset = 0;
    unsigned int i, file_size, need_comma;

    FILE *f_input, *f_output;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s binary_file output_file array_name\n",
                argv[0]);
        return -1;
    }
    if (argc > 4)
    {
        for(int x=4; x < argc;x++)
        {
            if (!strcmp(argv[x],"--define-size-as-macro"))
            {
                write_size_as_macro = 1;
                continue;
            }
            if (!strcmp(argv[x], "--create-guard"))
            {
                write_guard = 1;
                continue;
            }
            if (!strcmp(argv[x], "--add-hex-offset"))
            {
                add_hex_offset = 1;
                continue;
            }
        }
    }

    f_input = fopen(argv[1], "rb");
    if (f_input == NULL) {
        fprintf(stderr, "%s: can't open %s for reading\n", argv[0], argv[1]);
        return -1;
    }

    // Get the file length
    fseek(f_input, 0, SEEK_END);
    file_size = ftell(f_input);
    fseek(f_input, 0, SEEK_SET);

    buf = (char *) malloc(file_size);
    assert(buf);

    fread(buf, file_size, 1, f_input);
    fclose(f_input);

    f_output = fopen(argv[2], "w");
    if (f_output == NULL) {
        fprintf(stderr, "%s: can't open %s for writing\n", argv[0], argv[2]);
        return -1;
    }

    ident = argv[3];

    need_comma = 0;
    if (write_guard)
        fprintf(f_output, "#ifndef %s_DEFINED\n#define %s_DEFINED\n", ident, ident);
    
    fprintf(f_output, (write_size_as_macro) ? "#define %s_length %i\n":"const int %s_length = %i;\n", ident, file_size);
    fprintf(f_output, "const char %s[%s_length] = {", ident, ident);
    for (i = 0; i < file_size; ++i) {
        if (need_comma)
            fprintf(f_output, ", ");
        else
            need_comma = 1;
        if ((i % 16) == 0)
        {
            if (add_hex_offset)
                fprintf(f_output, "\n/%c0x%08x%c/", '*', i, '*');
            else
                fprintf(f_output, "\n\t");
        }
        fprintf(f_output, "0x%.2x", buf[i] & 0xff);
    }
    fprintf(f_output, "\n};\n\n");
    if (write_guard)
        fprintf(f_output, "#endif //%s_DEFINED\n\n", ident);
    fclose(f_output);

    return 0;
}
