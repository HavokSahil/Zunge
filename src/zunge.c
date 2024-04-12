#include "doc_parser.h"
#include "utils.h"

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        char *filename = argv[1];
        if (fileType(filename) == TYPE_EPUB)
        {
            int code = extractTextFromEPUB(filename);
            printf("%d\n", code);
            preprocessTextFile();
        }
        else if (fileType(filename) == TYPE_PDF)
        {
            int code = extractTextFromPDF(filename);
            printf("%d\n", code);
            preprocessTextFile();
        }
    }
    else
        printError("File name not provided");
}