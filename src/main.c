#include "epub_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "pdf_parser.h"
#include "tts.h"
#include <argp.h>
#include "interactive.h"
#include <sys/stat.h>
#include <sys/types.h>

#define INTERACTIVE 1
#define NOT_INTERACTIVE 0
#define NO_ARGS 1

// define program details
const char *arg_program_version = "Zunge 1.0.0";
const char *argp_program_bug_address = "https://github.com/havoksahil/zunge";
const char doc[] = "A Program to convert EBooks (PDF, EPUB) to mp3 file";

static struct argp_option options[] = {
    {"filename", 'f', "FILENAME", 0, "specify input filename"},
    {"outout", 'o', "OUTPUT", 0, "specify output filename"},
    {"bitrate", 'b', "BITRATE", 0, "(optional)specify bitrate"},
    {"files", 'F', "FILES", 0, "specify multiple input files"},
    {"interactive", 'i', 0, 0, "enable interactive reader"},
    {"help", 'h', 0, 0, "print help message"},
    {0}};

// Define structure to store parsed arguments
typedef struct
{
    char *filename;
    char *output;
    int bitrate;
    char **files;
    int interactive;
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
    case 'b':
        args->bitrate = atoi(arg);
        break;
    case 'F':
        args->files = &state->argv[state->next];
        state->next = state->argc;
        break;
    case 'i':
        args->interactive = 1;
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

void nonInteractivepanel(arguments args);
void InteractivePanel(arguments args);
void cacheConfig();

// Define argp parser
static struct argp argp = {options, parse_opt, NULL, doc};

int main(int argc, char **argv)
{
    arguments args;
    args.filename = NULL;
    args.output = "out.mp3";
    args.bitrate = 0;
    args.files = NULL;
    args.interactive = NOT_INTERACTIVE;

    argp_parse(&argp, argc, argv, 0, 0, &args);

    if (argc == NO_ARGS)
    {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "zunge");
        return 0;
    }

    switch (args.interactive)
    {
    case INTERACTIVE:
        printf("Yay! This is interactive time.\n");
        InteractivePanel(args);
        break;
    case NOT_INTERACTIVE:
        nonInteractivepanel(args);
        break;
    default:
        printf("Something unexpected happened.\n");
        break;
    }

    return 0;
}

void nonInteractivepanel(arguments args)
{

    int fileType = get_file_type(args.filename);
    char filename[128];
    char proc_filename[256];

    cacheConfig();

    switch (fileType)
    {
    case TYPE_PDF:
        readPDF(args.filename, filename);
        break;
    case TYPE_EPUB:
        extractEPUB(args.filename, filename);
        break;
    default:
        break;
    }
    sprintf(proc_filename, "%s%s", CACHE_PREFIX, PROC_OUTPUT_TEXT);
    text_file_to_voice(proc_filename, args.output);
}

void InteractivePanel(arguments args)
{
    int fileType = get_file_type(args.filename);
    char filename[128];

    cacheConfig();

    switch (fileType)
    {
    case TYPE_EPUB:
        extractEPUB(args.filename, filename);
        break;
    case TYPE_PDF:
        readPDF(args.filename, filename);
        break;
    default:
        break;
    }

    char dest[256];
    sprintf(dest, "%s%s", CACHE_PREFIX, PROC_OUTPUT_TEXT);
    interactive_window(dest);
}

void cacheConfig()
{
    printf("\nChecking for temp directory......");
    if (checkExistDirectory(CACHE_PREFIX) == SUCCESS)
    {
        printf("done\n");
    }
    else
    {
        printf("(not exist)\nCreating directory......");
        int result = mkdir(CACHE_PREFIX, 0777);
        if (result == SUCCESS)
        {
            printf("success\n");
        }
        else
        {
            printf("failure\n");
        }
    }
}