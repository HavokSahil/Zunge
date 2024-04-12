#include "utils.h"  // Custom utility header file
#include <string.h> // String manipulation
#include <dirent.h> // Directory manipulation
#include <stdlib.h> // General utilities
#include <ctype.h>  // Character classification
#include <stdio.h>  // Standard input/output
#include <regex.h>  // Regular Expression header file

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

// ============= Text Preprocessing Functions =============

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
    //
    regex_t reg;
    unsigned int replacements = 0;
    // if regex can't commpile pattern, do nothing
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
        } // if br is not equal to nmatch, leave
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

int removeWhiteSpace(char **str)
{
    const char *pattern = "\\s+";
    const char replace[] = " ";

    return regex_replace(str, pattern, replace);
}

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

    // const char *pattern5 = "[^\\w\\s]";
    // const char replace5[] = "";
    // int _5 = regex_replace(str, pattern5, replace5);

    return _1 & _2 & _3 & _4 & _5;
}

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
            fputs("> ", output_file);
            count = 0;
        }
        else
        {
            fputc(' ', output_file);
        }
        token = strtok(NULL, delimiter);
    }
}

int preprocessTextFile()
{
    if (!existFile(TEXT_EXTRACT_PATH))
    {
        printError("Output File doesn't exist.");
        return FAILURE;
    }

    FILE *fp;
    fp = fopen(TEXT_EXTRACT_PATH, "r");

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