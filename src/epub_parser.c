#include "epub_parser.h"
#include "utils.h"
#include <zip.h>
#include <stdio.h>
#include <stdlib.h>

void extractEPUB(char *filename, char *dest)
{
    int num_entries;
    zip_t *zip = NULL;
    char proc_text[256];

    printf("Opening EPUB file......");
    zip = zip_open(filename, 0, NULL);
    if (!zip)
    {
        printf("failed\n");
        exit(1);
    }
    printf("done\n");

    num_entries = zip_get_num_entries(zip, 0);

    sprintf(dest, "%s%s", CACHE_PREFIX, OUTPUT_TEXT);
    remove(dest);

    for (int i = 0; i < num_entries; ++i)
    {
        const char *entry_name = zip_get_name(zip, i, 0);
        if (is_html_file((char *)entry_name))
        {
            zip_file_t *file = zip_fopen_index(zip, i, 0);
            char buffer[1024];
            int bytes_read;
            char save_file[40] = CACHE_PREFIX;
            generateNameFromInt(save_file, i);
            FILE *fp = fopen(save_file, "w");
            if (!fp)
            {
                fprintf(stderr, "failed to save %s\n", save_file);
                continue;
            }
            while ((bytes_read = zip_fread(file, buffer, sizeof(buffer))))
            {
                fwrite(buffer, 1, bytes_read, fp);
            }
            fclose(fp);

            html_parser(save_file, dest);
            zip_fclose(file);
        }
    }

    sprintf(proc_text, "%s%s", CACHE_PREFIX, PROC_OUTPUT_TEXT);
    printf("Preprocessing text file......");
    preprocessTextFile(dest, proc_text);
    printf("done\n");
}