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
    sprintf(command, "python3 %s %s %s", PYTHON_SCRIPT_PATH, pdfFilePath, outputFilePath);

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
