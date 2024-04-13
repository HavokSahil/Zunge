#include "tts.h"       // Text-to-speech header
#include "utils.h"     // Utility functions header
#include <stdio.h>     // Standard I/O
#include <lame/lame.h> // LAME MP3 encoder
#include <stdlib.h>    // Standard library

// Convert text file to WAV
int textFileToWave()
{
    char command[COMMAND_LENGTH];
    sprintf(command, "text2wave %s -o %s > %s", PROCESSED_TEXT_PATH, WAV_FILE_PATH, LOG_FILE_PATH);
    int result = system(command);
    if (result == FAILURE)
    {
        return FAILURE;
    }
    return SUCCESS;
}

// Convert WAV to MP3
int waveToMp3(char *dest)
{
    FILE *pcm, *mp3;
    pcm = fopen(WAV_FILE_PATH, "rb");
    mp3 = fopen(dest, "wb");

    if (pcm == NULL || mp3 == NULL)
    {
        return FAILURE;
    }

    const int PCM_SIZE = 8192;
    short int pcm_buffer[PCM_SIZE * 2];
    unsigned char mp3_buffer[PCM_SIZE * 2];
    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, SAMPLE_RATE);
    lame_set_out_samplerate(lame, SAMPLE_RATE);
    lame_set_brate(lame, BIT_RATE);
    lame_set_mode(lame, STEREO);
    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);

    int read, write;

    do
    {
        read = fread(pcm_buffer, 2 * sizeof(short int), PCM_SIZE, pcm);
        if (read == SUCCESS)
        {
            write = lame_encode_flush(lame, mp3_buffer, PCM_SIZE * 2);
        }
        else
        {
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, PCM_SIZE * 2);
        }
        fwrite(mp3_buffer, write, 1, mp3);
    } while (read != 0);

    lame_close(lame);
    fclose(mp3);
    fclose(pcm);

    return SUCCESS;
}

// Speak text using Festival TTS
int speakText(char *text)
{
    char command[COMMAND_LENGTH];
    snprintf(command, sizeof(command), "echo %s | festival --tts > /dev/null", text);

    return system(command);
}
