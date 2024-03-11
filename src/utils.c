#include "utils.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <stdlib.h>

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
        if (cur_node->type == XML_ELEMENT_NODE && !xmlStrcmp(cur_node->name, (const xmlChar *)"p"))
        {
            xmlNode *text_node = cur_node->children;
            if (text_node && text_node->type == XML_TEXT_NODE)
            {
                fprintf(output_file, "%s\n", text_node->content);
                fseek(output_file, 0, SEEK_END);
            }
        }
        extract_p_tags(cur_node->children, output_file);
    }
}

void html_parser(char *read_file)
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
        FILE *output_file = fopen("output.txt", "a");
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
}