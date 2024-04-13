#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define SUCCESS 0
#define FAILURE 1
#define TRUE 1
#define FALSE 0
#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a < b) ? b : a)

// Supported File Types
#define TYPE_TXT 0
#define TYPE_PDF 1
#define TYPE_EPUB 2
#define TYPE_HTML 3
#define TYPE_UNSUPPORTED -1

#define PATH_EXIST 1
#define PATH_NOT_EXIST 0
#define BUFFER_SIZE 256
#define WORD_GROUP_SIZE 15

// Intermediate File Paths
#define CACHE_PREFIX "/tmp/_zunge_cache__/"
#define TEXT_EXTRACT_PATH "/tmp/_zunge_cache__/output.txt"
#define PROCESSED_TEXT_PATH "/tmp/_zunge_cache__/proc_output.txt"
#define TEMP_HTML_PATH "/tmp/_zunge_cache__/html_temp.html"
#define TEXT_COMPACT_PATH "/tmp/_zunge_cache__/text_compact.txt"
#define LOG_FILE_PATH "zunge.log"
#define WAV_FILE_PATH "/tmp/_zunge_cache__/zungeAudio.wav"

#define INITIAL_CAPACITY 100
typedef struct
{
    unsigned long int *array;
    long int size;
    long int capacity;
} DynamicArray;

int fileType(char *filename);
int existDirectory(char *path);
int isWhiteSpace(char *string);
void printError(char *string);
void printDebugMessage(char *string);
int existFile(char *filename);
char *getFilename(char *src);
void toLowerCase(char *text);
int regex_replace(char **str, const char *pattern, const char *replace);
void removeNonAlphanumeric(char *str);
int removeWhiteSpace(char **str);
int removeNewLines(char *str);
int replacePunctuation(char **str);
void tokenize(FILE *output_file, char *buffer);
int preprocessTextFile();
void printWarning(char *string);

DynamicArray *initDynamicArray();
void resizeArray(DynamicArray *arr);
void append(DynamicArray *arr, unsigned long int vaue);
unsigned long int get(DynamicArray *arr, long int index);
void freeDynamicArray(DynamicArray *arr);

#endif