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
