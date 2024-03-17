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
    char dest[256];
    char command[1024];
    char *filename;
    int result;

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
    filename = extractFilename(source);

    sprintf(dest, "/tmp/_zunge_cache__/wav/%s.wav", filename);
    sprintf(command, "text2wave %s -o /tmp/_zunge_cache__/wav/%s.wav >/dev/null 2>&1\n", source, filename); // Redirecting output to /dev/null
    printf("\nThis may take a while. Press Ctrl+C to exit.\n");
    printf("Generating Wave file......");
    fflush(stdout);
    result = system(command);
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
