#include "pdf_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include <sys/stat.h>
#include <sys/types.h>

char *readPDF(char *filename)
{
    FILE *fp;
    char path[128];
    char dest[268];
    char command[1024];
    char source[268];
    char executableDir[256];

    getExecutableDirectory(executableDir, sizeof(executableDir));
    sprintf(source, "%s/%s", executableDir, filename);
    sprintf(dest, "%s/%s", executableDir, "output.txt");
    sprintf(command, "python3 %s/pdf_to_text.py %s %s", executableDir, source, dest);

    printf("\nConverting PDF to text......");
    fp = popen(command, "r");
    if (fp == NULL)
    {
        printf("failed\n");
        exit(EXIT_FAILURE);
    }
    printf("done\n");

    fgets(path, sizeof(path) - 1, fp);
    if (strcmp(path, "0"))
    {
        printf("\nText file saved to: %s/%s\n\n", executableDir, dest);
    }
    else
    {
        fprintf(stderr, "Failed to parse PDF.\n");
        exit(EXIT_FAILURE);
    }

    pclose(fp);
    char *output_file = "output.txt";
    return output_file;
}