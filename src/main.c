#include "epub_parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: zunge <filename>\n");
        exit(1);
    }
    char *filename = argv[1];
    extractEPUB(filename);

    return 0;
}