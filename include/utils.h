#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>

#ifdef _WIN32
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#else
#include <unistd.h>
#endif

#define SUCCESS 0
#define FAILURE 1
#define TYPE_EPUB 1
#define TYPE_PDF 2
#define TYPE_TEXT 3
#define TEXT_WINDOW 1024
#define CACHE_PREFIX "/tmp/_zunge_cache__/"
#define OUTPUT_TEXT "output.txt"
#define PROC_OUTPUT_TEXT "proc_output.txt"

typedef struct
{
    float current_value;
    int max_value;
    char fill_char;
    char empty_char;
} ProgressBar;

void init_progress_bar(ProgressBar *bar, int max_value, char fill_char, char empty_char);
void update_progress_bar(ProgressBar *bar, float increment);
void draw_progress_bar(ProgressBar *bar);
void getExecutableDirectory(char *buffer, size_t bufferSize);
int is_html_file(char filename[]);
int get_file_type(char filename[]);
void generateNameFromInt(char name[], int n);
void extract_p_tags(xmlNode *node, FILE *output_file);
char *html_parser(char *source, char *dest);
size_t FindStringInBuffer(const char *buffer, const char *searchStr, size_t bufferLen);
void generateScriptFromText(char filename[]);
int checkExistDirectory(char *directoryPath);
int isWhiteSpace(char *candString);
char *extractFilename(char *source);
int wav2mp3(char *source, char *dest);
int preprocessTextFile(char *source, char dest[]);
void preprocessLine(char line[], char dest[]);
void interrupt_handler(int signum);
int max(int a, int b);
int min(int a, int b);

#endif