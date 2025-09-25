#pragma once

#include <cstdio>
#include <string>


extern int yyparse();
extern FILE* yyin;
extern void yyrestart(FILE *new_file);
extern int yylineno;
extern char *yytext;
extern std::string g_currentFileName;

class SwifoodLibrary;
extern SwifoodLibrary* g_swifood;