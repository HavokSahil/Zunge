#include "epub_parser.h"
#include "utils.h"
#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

char *extractEPUB(char *filename)
{
    printf("\nChecking for temp directory......");
    if (checkExistDirectory("/tmp/_zunge_cache__") == SUCCESS)
    {
        printf("done\n");
    }
    else
    {
        printf("(not exist)\nCreating directory......");
        int result = mkdir("/tmp/_zunge_cache__", 0777);
        if (result == SUCCESS)
        {
            printf("success\n");
        }
        else
        {
            printf("failure\n");
        }
    }

    printf("Opening EPUB file......");
    zip_t *zip = zip_open(filename, 0, NULL);
    if (!zip)
    {
        printf("failed\n");
        exit(1);
    }
    printf("done\n");

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
            html_parser(save_file);
            zip_fclose(file);
        }
    }

    char *output_file = "output.txt";
    return output_file;
}