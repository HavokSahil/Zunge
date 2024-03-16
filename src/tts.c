#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "utils.h"
#include "tts.h"

void text_file_to_voice(char *source, char *output)
{
    printf("Checking for directories.......");
    if (checkExistDirectory("/tmp/_zunge_cache__") == FAILURE)
    {
        if (mkdir("/tmp/_zunge_cache__", 0777) == FAILURE)
        {
            printf("failed\n");
            exit(FAILURE);
        }
        else
        {
            printf("success\n");
        }
    }
    else
    {
        printf("done\n");
    }
    mkdir("/tmp/_zunge_cache__/wav", 0777);
    // Extract filename from relative path of file
    char *filename = extractFilename(source);
    char dest[256];

    sprintf(dest, "/tmp/_zunge_cache__/wav/%s.wav", filename);

    char command[255];
    sprintf(command, "text2wave %s -o /tmp/_zunge_cache__/wav/%s.wav\n", source, filename);
    printf("\nThis may take a while. Press Ctrl+C to exit.\n");
    printf("Generating Wave file......");
    fflush(stdout);
    int result = system(command);
    if (result)
    {
        printf("(failed)\n");
    }
    else
    {
        printf("(done)\n");
    }

    printf("Compressing file......");
    fflush(stdout);
    if (wav2mp3(dest, output) == SUCCESS)
    {
        printf("done\n");
        printf("\nFile saved as %s\n", output);
    }
    else
        printf("failed\n");
}