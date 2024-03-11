#ifndef EPUB_PARSER_H
#define EPUB_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

typedef struct
{
    char *filename;
    char *content;
    size_t size;
} epub_file;

void extractEPUB(char *filename);

#endif
