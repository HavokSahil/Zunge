
# Zunge: How to make it
Zunge is an basically a text to audio converter with some enhanced functionality to enable it to read document files like PDF, EPUB and text files. It exports the audio file in mp3 format. It also includes an interactive reader which reads through document.

> **Note that we are not creating an application for speech generation from text**. But we are using opensource [`festival`](https://www.cstr.ed.ac.uk/projects/festival/) for speech generation. 
*Festival* is well suited for light weight free text to speech service with long text reader and document reader. It generates audio file in uncompressed *wave* file format, which is comparatively larger in size. Therefore for that we have used [`lame`](https://lame.sourceforge.io/) for audio compression from *.wav* to *.mp3*.

# Setup: How to Start

This project targets linux environment. And has been developed and tested in Ubuntu.
So our first step is going to be to install Ubuntu Operating System in Virtual Box.

### I. Setting up VirtualBox

For setup we will be downloading two files:
 - Start by downloading Virtual Box for Windows. [click here](https://download.virtualbox.org/virtualbox/7.0.14/VirtualBox-7.0.14-161095-Win.exe)
 - Download Ubuntu LTS .iso image [click here](https://ubuntu.com/download/desktop/thank-you?version=22.04.4&architecture=amd64)

Follow this [link](https://www.youtube.com/watch?v=v1JVqd8M3Yc) to setup VirtualBox with Ubuntu on Windows

### II. Installing Required Libraries
```bash
sudo apt update
sudo apt upgrade

# install required libraries
sudo apt-get install -y libzip-dev libxml2-dev cmake libmp3lame-dev \
    python3 python3-pip lame festival libavformat-dev libavcodec-dev \
    libavutil-dev  gedit && \
    pip3 install --no-cache-dir setuptools PyMuPDF

```

### III. Familiarising with CMAKE build tool.
![logo](https://github.com/HavokSahil/Zunge/assets/87008169/dada207b-8c1e-4c75-8b81-2b6b2d6316ae) 

For a large project with multiple libraries and dependencies we generally use build tools to make our project.
> *Imagine a project where you have 2000 lines of codes around 10-12 files. And you have finally managed to complete the project after 2 long weekends. Now you want to compile your project. Where will you start? Its gets disastorous quickly to link all dependencies together and compilation is too messy.*
*To avoid that and to ease our life we use build tools, **CMAKE** is just one of them. You just have to tell it what are the source files what are the libraries and dependencies and the headers and flags. It handles everything for you, there is just no reason why one shouldn't use it.*

**Note: You don't have to learn CMAKE now, just have an idea what it is. I will provide the code for it to you. Its not important to know it for this project.**

### IV. Flow of the Project
![graph](https://github.com/HavokSahil/Zunge/assets/87008169/e632e2fa-2ba6-4579-9793-757c15bfffd1)



### V. Project Directory Structure
Following is the project structure of Zunge
```md
zunge
├── CMakeLists.txt
├── Documentation
│   └── README.md
├── include
│   ├── doc_parser.h
│   ├── interactive.h
│   ├── tts.h
│   └── utils.h
├── README.md
└── src
    ├── doc_parser.c
    ├── interactive.c
    ├── pdf_to_text.py
    ├── tts.c
    ├── utils.c
    └── zunge.c

3 directories, 15 files
```
The root contains two folders and one file of convert
 - `src`: this folder contains all source files and implementations.
 - `include`: it contains all header files for function defination
 - `CMakeLists.txt`: this is the file for CMAKE build tool. You don't have to worry about it. We just put instructions here to compile our apparently long project

Use following code to create this project structure
```bash
mkdir Zunge
cd Zunge && mkdir src include && touch CMakeLists.txt
touch src/epub_parser.c src/interactive.c src/pdf_parser.c src/utils.c src/tts.c src/main.c src/pdf_to_text.py
touch include/epub_parser.h include/interactive.h include/pdf_parser.h include/utils.h include/tts.h
```

Now since the project directory is ready. You can start editing those files.

# Writing Code
Finally the second best of project. Writing Code!

I will be giving here the code for all the files and will be explaining each of them.


### 1 A utils.h (Code)
```c
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
```

### 1 A utils.h (Explanation)
Working on it

### 1 B utils.c (Code)
```c
#include "utils.h"  // Custom utility header file
#include <string.h> // String manipulation
#include <dirent.h> // Directory manipulation
#include <stdlib.h> // General utilities
#include <ctype.h>  // Character classification
#include <stdio.h>  // Standard input/output
#include <regex.h>  // Regular Expression header file
#include <sys/stat.h>
#include <sys/types.h>

// Function to determine the file type based on its extension
int fileType(char *filename)
{
    int len = strlen(filename);
    if ((len >= 5 && strcmp(filename + len - 5, ".html") == 0) || strcmp(filename + len - 4, ".htm") == 0)
        return TYPE_HTML;
    else if (len >= 5 && strcmp(filename + len - 5, ".epub") == 0)
        return TYPE_EPUB;
    else if (len >= 4 && strcmp(filename + len - 4, ".pdf") == 0)
        return TYPE_PDF;
    else if (len >= 4 && strcmp(filename + len - 4, ".txt") == 0)
        return TYPE_TXT;
    else
        return TYPE_UNSUPPORTED;
}

// Function to check if a directory exists
int existDirectory(char *path)
{
    DIR *dir = opendir(path);
    if (dir)
    {
        closedir(dir);
        return TRUE;
    }
    else
        return FALSE;
}

// Function to check if a string contains only white spaces
int isWhiteSpace(char *s)
{
    while (*s != '\0')
    {
        if (!isspace(*s))
            return FALSE;
        s++;
    }
    return TRUE;
}

// Function to extract the filename from a path
char *getFilename(char *src)
{
    char *filename = src;
    while (*src != '\0')
    {
        if (*src == '/')
            filename = ++src;
        src++;
    }
    return filename;
}

// Function to print error messages to standard error
void printError(char *s)
{
    fprintf(stderr, "ERROR: %s\n", s);
}

// Function to print debug messages
void printDebugMessage(char *s)
{
    printf("DEBUG: %s\n", s);
    fflush(stdout);
}

void printWarning(char *s)
{
    fprintf(stdout, "WARNING: %s\n", s);
}

// Function to check if file exist
int existFile(char *filename)
{
    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL)
    {
        return PATH_NOT_EXIST;
    }
    return PATH_EXIST;
}

// ============= Text Processing Functions =============

// Function to convert text to lowercase
void toLowerCase(char *text)
{
    for (int i = 0; text[i]; i++)
    {
        if (isalpha(text[i]))
        {
            text[i] = tolower(text[i]);
        }
    }
}

// Function to replace matches of a regex pattern with a replacement string in a given string
int regex_replace(char **str, const char *pattern, const char *replace)
{
    // Source: https://stackoverflow.com/questions/8044081/how-to-do-regex-string-replacements-in-pure-c
    // replaces regex in pattern with replacement observing capture groups
    // *str MUST be free-able, i.e. obtained by strdup, malloc, ...
    // back references are indicated by char codes 1-31 and none of those chars can be used in the replacement string such as a tab.
    // will not search for matches within replaced text, this will begin searching for the next match after the end of prev match
    // returns:
    //   -1 if pattern cannot be compiled
    //   -2 if count of back references and capture groups don't match
    //   otherwise returns number of matches that were found and replaced

    regex_t reg;
    unsigned int replacements = 0;

    // if regex can't compile pattern, return -1
    if (!regcomp(&reg, pattern, REG_EXTENDED))
    {
        size_t nmatch = reg.re_nsub;
        regmatch_t m[nmatch + 1];
        const char *rpl, *p;

        // count back references in replace
        int br = 0;
        p = replace;
        while (1)
        {
            while (*++p > 31)
                ;
            if (*p)
                br++;
            else
                break;
        }

        // if br is not equal to nmatch, return -2
        if (br != nmatch)
        {
            regfree(&reg);
            return -2;
        }

        // look for matches and replace
        char *new;
        char *search_start = *str;

        while (!regexec(&reg, search_start, nmatch + 1, m, REG_NOTBOL))
        {
            // make enough room
            new = (char *)malloc(strlen(*str) + strlen(replace));
            if (!new)
                exit(EXIT_FAILURE);
            *new = '\0';
            strncat(new, *str, search_start - *str);
            p = rpl = replace;
            int c;
            strncat(new, search_start, m[0].rm_so); // test before pattern
            for (int k = 0; k < nmatch; k++)
            {
                while (*++p > 31)
                    ;                       // skip printable char
                c = *p;                     // back reference (e.g. \1, \2, ...)
                strncat(new, rpl, p - rpl); // add head of rpl
                // concat match
                strncat(new, search_start + m[c].rm_so, m[c].rm_eo - m[c].rm_so);
                rpl = p++; // skip back reference, next match
            }
            strcat(new, p); // trailing of rpl
            unsigned int new_start_offset = strlen(new);
            strcat(new, search_start + m[0].rm_eo); // trailing text in *str
            // free(*str);
            *str = (char *)malloc(strlen(new) + 1);
            strcpy(*str, new);
            search_start = *str + new_start_offset;
            free(new);
            replacements++;
        }
        regfree(&reg);

        // adjust size
        *str = (char *)realloc(*str, strlen(*str) + 1);
        return replacements;
    }
    else
    {
        return -1;
    }
}

// Function to remove non-alphanumeric characters from a string
void removeNonAlphanumeric(char *str)
{
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++)
    {
        // Check if the character is alphanumeric
        if (isalnum(str[i]) || str[i] == ' ')
        {
            // If alphanumeric, keep the character
            str[j++] = str[i];
        }
    }
    // Add the null terminator to mark the end of the new string
    str[j] = '\0';
}

// Function to remove whitespace characters from a string
int removeWhiteSpace(char **str)
{
    const char *pattern = "\\s+";
    const char replace[] = " ";

    return regex_replace(str, pattern, replace);
}

// Function to remove newline characters from a string
int removeNewLines(char *str)
{
    int i;
    // Iterate through the string
    for (i = 0; str[i] != '\0'; i++)
    {
        // If the current character is '\n', replace it with space
        if (str[i] == '\n')
        {
            str[i] = ' ';
        }
    }
    return SUCCESS;
}

// Function to replace punctuation characters with appropriate replacements in a string
int replacePunctuation(char **str)
{
    const char *pattern1 = "&";
    const char replace1[] = " and ";
    int _1 = regex_replace(str, pattern1, replace1);

    const char *pattern2 = "+";
    const char replace2[] = " plus ";
    int _2 = regex_replace(str, pattern2, replace2);

    const char *pattern3 = "@";
    const char replace3[] = " at the rate ";
    int _3 = regex_replace(str, pattern3, replace3);

    const char *pattern4 = "#";
    const char replace4[] = " hash ";
    int _4 = regex_replace(str, pattern4, replace4);

    const char *pattern5 = "=";
    const char replace5[] = " equals ";
    int _5 = regex_replace(str, pattern5, replace5);

    return _1 & _2 & _3 & _4 & _5;
}

// Function to tokenize a string and write the tokens to a file
void tokenize(FILE *output_file, char *buffer)
{
    const char *delimiter = " ";
    char *token;

    token = strtok(buffer, delimiter);
    int count = 0;

    while (token != NULL)
    {
        fputs(token, output_file);
        count++;

        if (count >= WORD_GROUP_SIZE)
        {
            fputc('\n', output_file);
            count = 0;
        }
        else
        {
            fputc(' ', output_file);
        }
        token = strtok(NULL, delimiter);
    }
}

// Function to preprocess a text file
int preprocessTextFile()
{
    // Check if the output file exists
    if (!existFile(TEXT_EXTRACT_PATH))
    {
        printError("Output File doesn't exist.");
        return FAILURE;
    }

    FILE *fp;
    fp = fopen(TEXT_EXTRACT_PATH, "r");

    // Check if the output file can be opened
    if (fp == NULL)
    {
        printError("Could not open Output File.");
        return FAILURE;
    }

    FILE *op;
    op = fopen(TEXT_COMPACT_PATH, "w+");
    if (op == NULL)
    {
        printError("Could not open Compact Output File.");
        return FAILURE;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        char *_ = &buffer[0];
        toLowerCase(_);
        removeNewLines(_);
        removeWhiteSpace(&_);
        replacePunctuation(&_);
        fputs(_, op);
    }

    fclose(fp);

    fflush(op);
    fseek(op, 0, SEEK_SET);

    FILE *pop;
    pop = fopen(PROCESSED_TEXT_PATH, "w");
    if (pop == NULL)
    {
        printError("Could not open Processed Output File.");
        return FAILURE;
    }
    while (fgets(buffer, sizeof(buffer), op) != NULL)
    {
        removeNonAlphanumeric(buffer);
        tokenize(pop, buffer);
    }

    fclose(op);
    fclose(pop);

    return SUCCESS;
}

// ========= Dynamic Array Data Structure =========

// Function to initialize the dynamic array
DynamicArray *initDynamicArray()
{
    DynamicArray *arr = (DynamicArray *)malloc(sizeof(DynamicArray));
    if (arr == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }
    arr->array = (unsigned long int *)malloc(INITIAL_CAPACITY * sizeof(unsigned long int));
    if (arr->array == NULL)
    {
        printf("Memory allocation failed\n");
        free(arr);
        return NULL;
    }
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    return arr;
}

// Function to resize the dynamic array if needed
void resizeArray(DynamicArray *arr)
{
    if (arr->size >= arr->capacity)
    {
        arr->capacity *= 2;
        arr->array = (unsigned long int *)realloc(arr->array, arr->capacity * sizeof(unsigned long int));
        if (arr->array == NULL)
        {
            printf("Memory allocation failed\n");
            exit(1);
        }
    }
}

// Function to add an element to the dynamic array
void append(DynamicArray *arr, unsigned long int value)
{
    resizeArray(arr);
    arr->array[arr->size++] = value;
}

// Function to get an element from the dynamic array
unsigned long int get(DynamicArray *arr, long int index)
{
    if (index < 0 || index >= arr->size)
    {
        printf("Index out of bounds\n");
        exit(1);
    }
    return arr->array[index];
}

// Function to free the memory allocated for the dynamic array
void freeDynamicArray(DynamicArray *arr)
{
    free(arr->array);
    free(arr);
}
```

### 1 B utils.c (Explanation)
Working on it

### 2 A doc_parser.h (Code)

```c
#ifndef DOC_PARSER_H
#define DOC_PARSER_H

#include <libxml/parser.h>
#include <libxml/tree.h>

int extractTextFromEPUB(char *epubFilePath);
void convertHtmlTagsToText(xmlNode *node, FILE *outputFile);
void parseHtmlFile(char *htmlFilePath, char *outputFilePath);
char *generateHtmlFileNameFromIndex(int index, char *filenameBuffer);
int extractTextFromPDF(char *pdfFilePath);

#endif
```

### 2 A doc_parser.h (Explanation)
 Working on it.

### 2 B doc_parser.c (Code)

```c
#include "doc_parser.h"
#include "utils.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <zip.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// Definitions of Error Codes as in Python Code
#define SUCCESS_CODE "0"
#define ERROR_WRITE "1"
#define ERROR_MISSING_ARGS "2"
#define ERROR_MISSING_SOURCE "-1"

// Function prototypes
int extractTextFromEPUB(char *epubFilePath);
void convertHtmlTagsToText(xmlNode *node, FILE *outputFile);
void parseHtmlFile(char *htmlFilePath, char *outputFilePath);
char *generateHtmlFileNameFromIndex(int index, char *filenameBuffer);
int extractTextFromPDF(char *pdfFilePath);

// Function to extract text content from an EPUB file
int extractTextFromEPUB(char *epubFilePath)
{
    char *outputFilePath = TEXT_EXTRACT_PATH;
    // Open the EPUB file using libzip
    zip_t *epubArchive = zip_open(epubFilePath, 0, NULL);

    // Check if the file is successfully opened
    if (!epubArchive)
    {
        // Print error message if failed to open the EPUB file
        printError("Failed to open EPUB file.");
        // Return failure status
        return FAILURE;
    }

    // Get the number of entries (files) in the EPUB archive
    int numEntries = zip_get_num_entries(epubArchive, 0);

    // Remove the output file if it already exists
    remove(outputFilePath);

    // Loop through each entry in the EPUB archive
    for (int i = 0; i < numEntries; i++)
    {
        // Get the name of the current entry
        const char *entryName = zip_get_name(epubArchive, i, 0);

        // Check if the current entry is an HTML file
        if (fileType((char *)entryName) == TYPE_HTML)
        {
            // Open the HTML file from the EPUB archive
            zip_file_t *htmlFile = zip_fopen_index(epubArchive, i, 0);

            // Create a buffer to read the file content
            char buffer[1024];
            int bytesRead;
            char *tempHtmlFilePath = TEMP_HTML_PATH;

            // Open a new file to save the extracted HTML content
            FILE *tempHtmlFile = fopen(tempHtmlFilePath, "w");
            if (!tempHtmlFile)
            {
                // Print error message if failed to open the file
                printError("Failed to save EPUB entry.");
                continue;
            }

            // Read content from the HTML file and write it to the temporary file
            while ((bytesRead = zip_fread(htmlFile, buffer, sizeof(buffer))))
            {
                fwrite(buffer, 1, bytesRead, tempHtmlFile);
            }
            fclose(tempHtmlFile);

            // Parse the extracted HTML file
            parseHtmlFile(tempHtmlFilePath, outputFilePath);

            // Close the HTML file
            zip_fclose(htmlFile);
        }
    }

    // Close the EPUB archive
    zip_close(epubArchive);

    return SUCCESS;
}

// Function to convert HTML tags to plain text
void convertHtmlTagsToText(xmlNode *node, FILE *outputFile)
{
    // Iterate through each node in the HTML tree
    for (xmlNode *curNode = node; curNode; curNode = curNode->next)
    {
        // Get the text content of the current node
        xmlNode *textNode = curNode->children;
        if (textNode && textNode->type == XML_TEXT_NODE && !isWhiteSpace((char *)textNode->content))
        {
            // Convert HTML tags to plain text and write to the output file
            if (!xmlStrcmp(curNode->name, (const xmlChar *)"h1") ||
                !xmlStrcmp(curNode->name, (const xmlChar *)"h2") ||
                !xmlStrcmp(curNode->name, (const xmlChar *)"h3") ||
                !xmlStrcmp(curNode->name, (const xmlChar *)"h4"))
            {
                fprintf(outputFile, "%s\n", textNode->content);
            }
            else
            {
                fprintf(outputFile, "%s\n", textNode->content);
            }
            // Ensure the file pointer is at the end for appending
            fseek(outputFile, 0, SEEK_END);
        }
        // Recursively call the function for child nodes
        convertHtmlTagsToText(curNode->children, outputFile);
    }
}

// Function to parse HTML file
void parseHtmlFile(char *htmlFilePath, char *outputFilePath)
{
    // Parse the HTML file using libxml2
    xmlDoc *htmlDoc = xmlReadFile(htmlFilePath, NULL, 0);

    // Check if the file is successfully parsed
    if (htmlDoc == NULL)
    {
        // Print error message if failed to parse the HTML file
        printError("Could not parse HTML file.");
        return;
    }

    // Open the output file to write parsed content
    FILE *outputFile = fopen(outputFilePath, "a");
    if (outputFile == NULL)
    {
        // Print error message if failed to open the output file
        printError("Could not open output file.");
        xmlFreeDoc(htmlDoc);
        return;
    }

    // Get the root element of the parsed HTML file
    xmlNode *rootElement = xmlDocGetRootElement(htmlDoc);

    // Convert HTML tags to plain text and write to the output file
    convertHtmlTagsToText(rootElement, outputFile);

    // Close the output file
    fclose(outputFile);
    // Free the memory allocated for the parsed XML document
    xmlFreeDoc(htmlDoc);
    // Clean up the libxml2 parser
    xmlCleanupParser();
}

// Function to generate HTML file name from index
char *generateHtmlFileNameFromIndex(int index, char *filenameBuffer)
{
    // Generate HTML file name using index
    sprintf(filenameBuffer, "%d.html", index);
    return filenameBuffer;
}

// Function to parse PDF file and extract text
int extractTextFromPDF(char *pdfFilePath)
{
    FILE *fp;

    // Destination path for the extracted text
    char *outputFilePath = TEXT_EXTRACT_PATH;

    // Construct the command to execute the Python script
    char command[512];
    sprintf(command, "python3 pdf_to_text.py %s %s", pdfFilePath, outputFilePath);

    // Open a pipe to execute the command
    fp = popen(command, "r");
    if (fp == NULL)
    {
        // Print error message if failed to open pipe
        printError("Failed to open pipe.");
        return FAILURE;
    }

    // Read the exit code from the Python script
    char exit_code[128];
    fgets(exit_code, sizeof(exit_code) - 1, fp);

    // Check the exit code and handle errors accordingly
    if (!strcmp(exit_code, SUCCESS_CODE))
    {
        if (strcmp(exit_code, ERROR_WRITE) == 0)
        {
            printError("Failed to write file as destination.");
        }
        else if (strcmp(exit_code, ERROR_MISSING_SOURCE) == 0)
        {
            printError("Missing source PDF file.");
        }
        else if (strcmp(exit_code, ERROR_MISSING_ARGS) == 0)
        {
            printError("Insufficient number of arguments");
        }

        // Close the pipe
        pclose(fp);
        return FAILURE;
    }

    // Close the pipe
    pclose(fp);
    return SUCCESS;
}
```

### 3 A tts.h (Code)

```c
#ifndef TTS_H
#define TTS_H

#define COMMAND_LENGTH 1024
#define SAMPLE_RATE 7864
#define BIT_RATE 128

int textFileToWave();
int waveToMp3(char *dest);
int speakText(char *text);

#endif
```

# 3 A tts.h (Explanation)
Working on it

# 3 B tts.c (Code)
```c
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
```

### 3 B tts.c (Explanation)
Working on it

### 4 A interactive.h (Code)
```c
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
```

### 4 A interactive.h (Explanation)
Working on it

### 4 B interactive.c (Code)
```c
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

int streamScreen(int theme, int readMode)
{

    Settings *settings = initialiseSettings();

    if (settings == NULL)
    {
        return FAILURE;
    }

    // Set the theme configuration
    settings->theme = theme;
    settings->readMode = readMode;

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
```
### 4 B interactive.c (Explanation)
Working on it

### 5 zunge.c (Code)
```c
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "tts.h"
#include <argp.h>
#include "interactive.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "doc_parser.h"

#define INTERACTIVE 1
#define NOT_INTERACTIVE 0
#define NO_ARGS 1

// define program details
const char *arg_program_version = "Zunge 1.0.0";
const char *argp_program_bug_address = "https://github.com/havoksahil/zunge";
const char doc[] = "A Program to convert EBooks (PDF, EPUB) to mp3 file";

static struct argp_option options[] = {
    {"input", 'f', "FILENAME", 0, "specify input filename"},
    {"output", 'o', "OUTPUT", 0, "specify output filename"},
    {"interactive", 'i', 0, 0, "enable interactive reader"},
    {"theme", 't', "THEME", 0, "Choose a theme (0-7). Themes:\n"
                               "0: Dark theme with green highlight.\n"
                               "1: Dark theme with cyan highlight.\n"
                               "2: Dark theme with white highlight.\n"
                               "3: Dark theme with magenta highlights.\n"
                               "4: Dark theme with yellow highlights.\n"
                               "5: Light theme with red highlights.\n"
                               "6: Light theme with blue highlights.\n"
                               "7: Light theme with black highlights."},
    {"read", 's', 0, 0, "turn on reading in interactive mode."},
    {"help", 'h', 0, 0, "print help message"},
    {0}};

// Define structure to store parsed arguments
typedef struct
{
    char *filename;
    char *output;
    int interactive;
    int theme;
    int readMode;
} arguments;

// Parse command-line arguments
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    arguments *args = state->input;
    switch (key)
    {
    case 'f':
        args->filename = arg;
        break;
    case 'o':
        args->output = arg;
        break;
    case 'i':
        args->interactive = 1;
        break;
    case 't':
        args->theme = atoi(arg);
        if (!(args->theme <= 8 && args->theme >= 0))
            args->theme = 0;
        break;
    case 's':
        args->readMode = 1;
        break;
    case 'h':
        argp_state_help(state, stdout, ARGP_HELP_STD_HELP);

        break;
    case ARGP_KEY_ARG:
        argp_usage(state);
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int launchConverter(arguments args);
int launchInteractivePanel(arguments args);
int checkConfiguration();

// Define argp parser
static struct argp argp = {options, parse_opt, NULL, doc};

int main(int argc, char **argv)
{
    arguments args;
    args.filename = NULL;
    args.output = "out.mp3";
    args.interactive = NOT_INTERACTIVE;
    args.theme = THEME_DARK_GREEN;

    argp_parse(&argp, argc, argv, 0, 0, &args);

    if (argc == NO_ARGS)
    {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "zunge");
        return 0;
    }

    checkConfiguration();

    switch (args.interactive)
    {
    case INTERACTIVE:
        launchInteractivePanel(args);
        break;
    case NOT_INTERACTIVE:
        launchConverter(args);
        break;
    default:
        printError("Unsupported argument.");
        break;
    }

    return 0;
}

int launchConverter(arguments args)
{
    int okParse;

    switch (fileType(args.filename))
    {
    case TYPE_PDF:
        okParse = extractTextFromPDF(args.filename);
        break;
    case TYPE_EPUB:
        okParse = extractTextFromEPUB(args.filename);
        break;
    default:
        okParse = FAILURE;
        break;
    }

    if (okParse == FAILURE)
    {
        return FAILURE;
    }

    if (preprocessTextFile() == FAILURE)
        return FAILURE;

    if (textFileToWave() == FAILURE)
        return FAILURE;

    if (waveToMp3(args.output) == FAILURE)
        return FAILURE;

    return SUCCESS;
}

int launchInteractivePanel(arguments args)
{
    int okParse;

    switch (fileType(args.filename))
    {
    case TYPE_EPUB:
        okParse = extractTextFromEPUB(args.filename);
        break;
    case TYPE_PDF:
        okParse = extractTextFromPDF(args.filename);
        break;
    default:
        break;
    }

    if (okParse == FAILURE)
    {
        return FAILURE;
    }

    if (preprocessTextFile() == FAILURE)
    {
        return FAILURE;
    }

    if (streamScreen(args.theme, args.readMode) == FAILURE)
    {
        return FAILURE;
    }

    return SUCCESS;
}

int checkConfiguration()
{
    const char *path = CACHE_PREFIX;

    if (mkdir(path, 0755) == -1)
    {
        // printError("Cache directory missing.");
        return FAILURE;
    }

    return SUCCESS;
}
```

### zunge.c (Explanation)
Working on it.
