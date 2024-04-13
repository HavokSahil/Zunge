#include <stdio.h>
#include <ncurses.h>
#include "utils.h"
#include <string.h>
#include "tts.h"
#include "interactive.h"
#include <stdlib.h>

#define MAX_LINE_LENGTH 256
#define READ_FORWARD 1
#define READ_BACKWARD -1

#define TITLE_TEXT "Zunge Interactive Reader"

#define SILENT_READER 0
#define VOICED_READER 1

// All Theme Options
#define THEME_DARK_GREEN 0
#define THEME_DARK_CYAN 1
#define THEME_DARK_WHITE 2
#define THEME_DARK_MAGENTA 3
#define THEME_DARK_YELLOW 4
#define THEME_LIGHT_RED 5
#define THEME_LIGHT_BLUE 6
#define THEME_LIGHT_BLACK 7

// Function to initialize the interactive screen

Settings *initialiseSettings()
{
    Settings *settings = (Settings *)malloc(sizeof(Settings));

    if (settings == NULL)
    {
        printError("Could not allocate memory.");
        return NULL;
    }

    DynamicArray *arr = initDynamicArray();

    settings->readMode = VOICED_READER;
    settings->theme = THEME_LIGHT_BLACK;
    settings->lineSize = MAX_LINE_LENGTH;

    FILE *fp;
    fp = fopen(PROCESSED_TEXT_PATH, "r");

    if (fp == NULL)
    {
        printError("Processed Output File Doesn't Exist.");
        return NULL;
    }

    unsigned long int nlines = 0;
    char line[MAX_LINE_LENGTH];
    append(arr, 0);
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        nlines++;
        unsigned long int pos = ftell(fp);
        append(arr, pos);
    }

    settings->mode = READ_FORWARD;
    settings->nLines = nlines;
    settings->lineMap = arr;

    fclose(fp);
    return settings;
}

void freeSettings(Settings *settings)
{
    freeDynamicArray(settings->lineMap);
    free(settings);
}

int testSettings()
{
    Settings *settings = initialiseSettings();
    printf("%ld\n", settings->nLines);
    for (long int i = 0; i < settings->nLines; i++)
    {
        printf("%ld ", get(settings->lineMap, i));
    }
    freeSettings(settings);

    return SUCCESS;
}

int streamScreen()
{

    Settings *settings = initialiseSettings();

    if (settings == NULL)
    {
        return FAILURE;
    }

    // Open the file for reading
    FILE *fp;
    fp = fopen(PROCESSED_TEXT_PATH, "r");

    // Check if the file opened successfully
    if (fp == NULL)
    {
        printError("Processed Output File Doesn't Exist.");
        return FAILURE;
    }

    // Initialize ncurses environment
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Check if terminal supports colors
    if (has_colors() == FALSE)
    {
        printWarning("Support for Colors not found.");
    }
    else
    {
        start_color();
        if (settings->theme == THEME_DARK_GREEN)
        {
            init_pair(1, COLOR_GREEN, COLOR_BLACK);
            init_pair(2, COLOR_RED, COLOR_BLACK);
            init_pair(3, COLOR_BLUE, COLOR_BLACK);
            init_pair(4, COLOR_WHITE, COLOR_BLACK);
        }
        else if (settings->theme == THEME_DARK_MAGENTA)
        {
            init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
            init_pair(2, COLOR_GREEN, COLOR_BLACK);
            init_pair(3, COLOR_RED, COLOR_BLACK);
            init_pair(4, COLOR_WHITE, COLOR_BLACK);
        }
        else if (settings->theme == THEME_DARK_CYAN)
        {
            init_pair(1, COLOR_CYAN, COLOR_BLACK);
            init_pair(2, COLOR_BLUE, COLOR_BLACK);
            init_pair(3, COLOR_GREEN, COLOR_BLACK);
            init_pair(4, COLOR_WHITE, COLOR_BLACK);
        }
        else if (settings->theme == THEME_DARK_WHITE)
        {
            init_pair(1, COLOR_WHITE, COLOR_BLACK);
            init_pair(2, COLOR_BLUE, COLOR_BLACK);
            init_pair(3, COLOR_GREEN, COLOR_BLACK);
            init_pair(4, COLOR_RED, COLOR_BLACK);
        }
        else if (settings->theme == THEME_LIGHT_BLACK)
        {
            init_pair(1, COLOR_BLACK, COLOR_WHITE);
            init_pair(2, COLOR_BLUE, COLOR_WHITE);
            init_pair(3, COLOR_MAGENTA, COLOR_WHITE);
            init_pair(4, COLOR_BLACK, COLOR_WHITE);
        }
        else if (settings->theme == THEME_LIGHT_BLUE)
        {
            init_pair(1, COLOR_BLUE, COLOR_WHITE);
            init_pair(2, COLOR_BLACK, COLOR_WHITE);
            init_pair(3, COLOR_MAGENTA, COLOR_WHITE);
            init_pair(4, COLOR_RED, COLOR_WHITE);
        }
        else if (settings->theme == THEME_LIGHT_RED)
        {
            init_pair(1, COLOR_RED, COLOR_WHITE);
            init_pair(2, COLOR_BLACK, COLOR_WHITE);
            init_pair(3, COLOR_MAGENTA, COLOR_WHITE);
            init_pair(4, COLOR_BLUE, COLOR_WHITE);
        }
    }

    bkgd(COLOR_PAIR(4));

    // Get dimensions of the terminal window
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Calculate center coordinates
    int y_center = rows / 2;
    int x_center = (cols - 11) / 2; // Adjusted for text centering

    // Buffers to store lines of text
    char line[MAX_LINE_LENGTH];
    char prevText[MAX_LINE_LENGTH] = "";
    char prevPrevText[MAX_LINE_LENGTH] = "";

    // Read lines from the file and display them
    long int index = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // Skip empty lines
        if (isWhiteSpace(line) == TRUE)
        {
            continue;
        }

        // Clear the screen
        erase();

        // Print title at the top
        attron(A_BOLD | COLOR_PAIR(2));
        mvprintw(1, (cols - strlen(TITLE_TEXT)) / 2, TITLE_TEXT);

        // Display previous, current, and next lines of text
        int start_col1 = max(0, (x_center - strlen(prevPrevText) / 2));
        attron(A_BOLD | COLOR_PAIR(4));
        mvprintw(y_center - 3, start_col1, "%s", prevPrevText);

        attron(A_BOLD | COLOR_PAIR(4));
        int start_col2 = max(0, (x_center - strlen(prevText) / 2));
        mvprintw(y_center, start_col2, "%s", prevText);

        attron(A_BOLD | COLOR_PAIR(1));
        int start_col3 = max(0, (x_center - strlen(line) / 2));
        mvprintw(y_center + 3, start_col3, "%s", line);

        // Display instructions at the bottom of the screen
        attron(COLOR_PAIR(3));
        mvprintw(rows - 4, 0, "Hold q at end of line to quit. Use 'n' for next, 'p' for previous.");
        refresh();

        if (settings->readMode == VOICED_READER)
        {
            removeNewLines(line);
            speakText(line);
        }

        // Wait for user input
        char ch = getch();
        if (ch == 'q')
        {             // ASCII code for 'q'
            endwin(); // End curses mode before exiting
            fclose(fp);
            freeSettings(settings);
            return SUCCESS;
        }
        else if (ch == 'n')
        {
            settings->mode = READ_FORWARD; // Move forward
        }
        else if (ch == 'p')
        {
            settings->mode = READ_BACKWARD; // Move backward
        }

        // Update buffers for next iteration based on direction

        if (settings->mode == READ_FORWARD)
        {
            // Code for forward reading
            strncpy(prevPrevText, prevText, sizeof(prevPrevText));
            strncpy(prevText, line, sizeof(prevText));
            index++;
        }
        else
        {
            // Code for backward reading
            index--;

            // for prevPrevText (index-2)
            if (index - 2 >= 0)
            {
                unsigned long int pos = get(settings->lineMap, index - 2);
                fseek(fp, pos, SEEK_SET);
                fgets(prevPrevText, MAX_LINE_LENGTH, fp);
            }
            else
            {
                sprintf(prevPrevText, (const char *)" ");
            }

            // for prevText (index-1)
            if (index - 1 >= 0)
            {
                unsigned long int pos = get(settings->lineMap, index - 1);
                fseek(fp, pos, SEEK_SET);
                fgets(prevText, MAX_LINE_LENGTH, fp);
            }
            else
            {
                sprintf(prevText, (const char *)" ");
            }

            if (index <= 0)
            {
                index = 0;
                fseek(fp, 0, SEEK_SET);
            }
        }
    }

    // End curses mode and close the file
    endwin();
    freeSettings(settings);
    fclose(fp);

    return SUCCESS;
}
