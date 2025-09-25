#include "swifood.hpp"


void SwifoodLibrary::load(const std::string& fileName)
{
    FILE* inputFile = fopen(fileName.c_str(), "r");

    if (inputFile == nullptr) {
        std::cerr << "Error: Cannot open file " << fileName << std::endl;
        return;
    }

    yylineno = 1;
    yyrestart(inputFile);
    g_currentFileName = fileName;

    yyparse();

    fclose(inputFile);
}
