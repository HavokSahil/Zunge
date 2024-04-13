#ifndef TTS_H
#define TTS_H

#define COMMAND_LENGTH 1024
#define SAMPLE_RATE 7864
#define BIT_RATE 128

int textFileToWave();
int waveToMp3(char *dest);
int speakText(char *text);

#endif
