#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#define READ_FORWARD 1
#define READ_BACKWARD -1

#include "utils.h"

struct settings_structure
{
    int readMode;
    int mode;
    int theme;
    int lineSize;
    unsigned long int nLines;
    DynamicArray *lineMap;
};

typedef struct settings_structure Settings;

int streamScreen();
Settings *initialiseSettings();
int testSettings();

#endif