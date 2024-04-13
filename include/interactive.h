#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#define READ_FORWARD 1
#define READ_BACKWARD -1

// All Theme Options
#define THEME_DARK_GREEN 0
#define THEME_DARK_CYAN 1
#define THEME_DARK_WHITE 2
#define THEME_DARK_MAGENTA 3
#define THEME_DARK_YELLOW 4
#define THEME_LIGHT_RED 5
#define THEME_LIGHT_BLUE 6
#define THEME_LIGHT_BLACK 7

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

int streamScreen(int theme, int readMode);
Settings *initialiseSettings();
int testSettings();

#endif