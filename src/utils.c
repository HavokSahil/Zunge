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