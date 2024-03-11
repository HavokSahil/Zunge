#include "utils.h"

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
