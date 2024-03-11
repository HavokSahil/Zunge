#include "epub_parser.h"
#include "utils.h"
#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

void extractEPUB(char *filename)
{

    int result = mkdir("/tmp/_zunge_cache__", 0777);
    printf("The result is %d\n", result);
    zip_t *zip = zip_open(filename, 0, NULL);
    if (!zip)
    {
        fprintf(stderr, "Error opening EPUB file: %s\n", filename);
        exit(1);
    }
    int num_entries = zip_get_num_entries(zip, 0);

    for (int i = 0; i < num_entries; ++i)
    {
        const char *entry_name = zip_get_name(zip, i, 0);
        if (is_html_file((char *)entry_name))
        {
            zip_file_t *file = zip_fopen_index(zip, i, 0);
            char buffer[1024];
            int bytes_read;
            char save_file[40] = "/tmp/_zunge_cache__/";
            generateNameFromInt(save_file, i);
            printf("%s\n", save_file);
            FILE *fp = fopen(save_file, "w");
            if (!fp)
            {
                fprintf(stderr, "Error creating file %s\n", save_file);
                continue;
            }
            while ((bytes_read = zip_fread(file, buffer, sizeof(buffer))))
            {
                fwrite(buffer, 1, bytes_read, fp);
            }
            fclose(fp);
            zip_fclose(file);
        }
    }
}