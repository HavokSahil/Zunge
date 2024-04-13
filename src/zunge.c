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
