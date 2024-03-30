
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
Zunge/
├── CMakeLists.txt
├── include/
│   ├── epub_parser.h
│   ├── interactive.h
│   ├── pdf_parser.h
│   ├── tts.h
│   └── utils.h
├── src/
│   ├── epub_parser.c
│   ├── interactive.c
│   ├── main.c
│   ├── pdf_parser.c
│   ├── pdf_to_text.py
│   ├── tts.c
│   └── utils.c
└── structure.md
```
The root contains two folders and one file
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
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>
#include <unistd.h>

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
```
### 1 A utils.h (Explanation)
Working on it

### 1 B utils.c (Code)
```c
#include "utils.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <lame/lame.h>

#define SAMPLE_RATE 7864
#define BIT_RATE 128

int is_html_file(char filename[])
{
    int len = strlen(filename);
    if (len >= 5 && (strcmp(filename + len - 5, ".html") == 0 || strcmp(filename + len - 4, "htm") == 0))
    {
        return 1;
    }
    else
        return 0;
}

int get_file_type(char filename[])
{
    int len = strlen(filename);
    if (len >= 5 && (strcmp(filename + len - 5, ".epub") == 0))
    {
        return 1;
    }
    else if (len >= 5 && (strcmp(filename + len - 4, ".pdf") == 0))
    {
        return 2;
    }
    else if (len >= 4 && (strcmp(filename + len - 4, ".txt") == 0))
    {
        return 3;
    }
    else
        return -1;
}

void generateNameFromInt(char str[], int n)
{
    char temp[20] = "";
    sprintf(temp, "%d.html", n);
    strcat(str, temp);
}

void extract_p_tags(xmlNode *node, FILE *output_file)
{
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE && (!xmlStrcmp(cur_node->name, (const xmlChar *)"p") || !xmlStrcmp(cur_node->name, (const xmlChar *)"h1") || !xmlStrcmp(cur_node->name, (const xmlChar *)"h2") || !xmlStrcmp(cur_node->name, (const xmlChar *)"h3")))
        {
            xmlNode *text_node = cur_node->children;
            if (text_node && text_node->type == XML_TEXT_NODE)
            {
                if (!xmlStrcmp(cur_node->name, (const xmlChar *)"h1"))
                {
                    fprintf(output_file, "%s\n\n\n\n", text_node->content);
                }
                else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"h2"))
                {
                    fprintf(output_file, "%s\n\n\n", text_node->content);
                }
                else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"h3"))
                {
                    fprintf(output_file, "%s\n\n", text_node->content);
                }
                else
                {
                    fprintf(output_file, "%s\n", text_node->content);
                }
                fseek(output_file, 0, SEEK_END);
            }
        }
        extract_p_tags(cur_node->children, output_file);
    }
}

char *html_parser(char *read_file, char *save_file)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    // Load HTML file
    doc = xmlReadFile(read_file, NULL, 0);

    if (doc == NULL)
    {
        fprintf(stderr, "Could not parse file.\n");
    }

    else
    {
        FILE *output_file = fopen(save_file, "a");
        if (output_file == NULL)
        {
            fprintf(stderr, "Error: could not open output file\n");
            xmlFreeDoc(doc);
            exit(1);
        }

        // Get the root element
        root_element = xmlDocGetRootElement(doc);

        // Extract content of <p> tags
        extract_p_tags(root_element, output_file);

        fclose(output_file);

        // Free the document
        xmlFreeDoc(doc);

        xmlCleanupParser();
    }
    char *output_filename = "output.txt";
    return output_filename;
}

size_t FindStringInBuffer(const char *buffer, const char *searchStr, size_t bufferLen)
{
    const char *foundStr = strstr(buffer, searchStr);
    if (foundStr != NULL)
    {
        return foundStr - buffer;
    }
    else
    {
        return bufferLen;
    }
}

// Function to get the directory of the executable
void getExecutableDirectory(char *buffer, size_t bufferSize)
{
#ifdef _WIN32
    GetModuleFileName(NULL, buffer, bufferSize);
    PathRemoveFileSpec(buffer);
#else
    ssize_t len = readlink("/proc/self/exe", buffer, bufferSize);
    if (len != -1)
    {
        buffer[len] = '\0';
        char *lastSlash = strrchr(buffer, '/');
        if (lastSlash != NULL)
        {
            *lastSlash = '\0'; // Remove the executable name
        }
    }
#endif
}

void generateScriptFromText(char filename[])
{
    printf("%s\n", filename);
}

void init_progress_bar(ProgressBar *bar, int max_value, char fill_char, char empty_char)
{
    bar->current_value = 0;
    bar->max_value = max_value;
    bar->fill_char = fill_char;
    bar->empty_char = empty_char;
}

void update_progress_bar(ProgressBar *bar, float increment)
{
    bar->current_value = increment;
    if (bar->current_value > bar->max_value)
    {
        bar->current_value = bar->max_value;
    }
}

void draw_progress_bar(ProgressBar *bar)
{
    int i;
    printf("The value of current_value is %.3f\n", bar->current_value);
    printf("The value of total is %d\n", bar->max_value);
    for (i = 0; i < (int)bar->current_value; i++)
    {
        printf("%c", bar->fill_char);
    }
    for (i = (int)bar->current_value; i > bar->max_value; i++)
    {
        printf("%c", bar->empty_char);
    }
    float progressPercentage = bar->current_value / bar->max_value;
    printf("The progress percentage is %.3f\n", progressPercentage);
    printf("%.2f%%\n", progressPercentage * 100);
}

int checkExistDirectory(char *directoryPath)
{
    DIR *dir = opendir(directoryPath);
    if (dir)
    {
        closedir(dir);
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

int isWhiteSpace(char *candString)
{
    while (*candString != '\0')
    {
        if (!isspace(*candString))
        {
            return FAILURE;
        }
        candString++;
    }
    return SUCCESS;
}

char *extractFilename(char *filename)
{
    char *result;
    result = filename;
    while (*filename != '\0')
    {
        if (*filename == '/')
            result = ++filename;
        filename++;
    }
    return result;
}

int wav2mp3(char *source, char *dest)
{
    int read, write;
    FILE *pcm = fopen(source, "rb");
    FILE *mp3 = fopen(dest, "wb");

    if (pcm == NULL || mp3 == NULL)
    {
        return EXIT_FAILURE;
    }

    const int PCM_SIZE = 8192;
    short int pcm_buffer[PCM_SIZE * 2];
    unsigned char mp3_buffer[PCM_SIZE * 2];

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, SAMPLE_RATE);  // Set input sample rate
    lame_set_out_samplerate(lame, SAMPLE_RATE); // Set output sample rate (half of input)
    lame_set_brate(lame, BIT_RATE);             // Set bitrate to 128 kbps
    lame_set_mode(lame, STEREO);                // Set stereo mode
    lame_set_VBR(lame, vbr_default);            // Use default VBR mode
    lame_init_params(lame);

    do
    {
        read = fread(pcm_buffer, 2 * sizeof(short int), PCM_SIZE, pcm);
        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, PCM_SIZE * 2);
        else
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, PCM_SIZE * 2);

        fwrite(mp3_buffer, write, 1, mp3);
    } while (read != 0);

    lame_close(lame);
    fclose(mp3);
    fclose(pcm);

    return SUCCESS;
}

int isAllowedPunctuation(char c)
{
    switch (c)
    {
    case ',':
    case '.':
    case '!':
    case '"':
    case ';':
        return 1;
    default:
        return 0;
    }
}

void preprocessLine(char line[], char dest[])
{
    int i = 0, j = 0;
    char prev = '\0';
    while (line[i] != '\0' && j < TEXT_WINDOW - 1)
    {
        if (isAllowedPunctuation(line[i]) || line[i] == ' ' || line[i] == '\n' || line[i] == '\t' || isalpha(line[i]) || isdigit(line[i]))
        {
            if (line[i] == '\n' && prev == '\n')
            {
                prev = line[i];
            }
            else
            {
                dest[j++] = line[i];
            }
        }
        i++;
    }
    if (j < TEXT_WINDOW)
    {
        dest[j] = '\0';
    }
    else
    {
        dest[TEXT_WINDOW - 1] = '\0';
    }
}

int preprocessTextFile(char *source, char dest[])
{
    FILE *inputFile = fopen(source, "r");
    if (inputFile == NULL)
    {
        return FAILURE;
    }

    FILE *outputFile = fopen(dest, "w");
    if (outputFile == NULL)
    {
        fclose(inputFile);
        return FAILURE;
    }

    char line[TEXT_WINDOW];
    char proc_line[TEXT_WINDOW];
    while (fgets(line, sizeof(line), inputFile) != NULL)
    {
        preprocessLine(line, proc_line);
        fputs(proc_line, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);
    return SUCCESS;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}
int min(int a, int b)
{
    return (a < b) ? a : b;
}
```

### 1 B utils.c (Explanation)
Working on it
