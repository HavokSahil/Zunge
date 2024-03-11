#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdlib.h>

int is_html_file(char filename[]);
void generateNameFromInt(char name[], int n);
void extract_p_tags(xmlNode *node, FILE *output_file);
void html_parser(char *);
#endif