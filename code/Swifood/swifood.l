%{
#include "swifood.hpp"
#include "globals.hpp"
#include "recipe.hpp"
#include <string>
#include <vector>
#include <map>
#include <any>
#include "swifood.tab.hpp"

#undef YY_DECL
#define YY_DECL extern "C" int yylex()
%}

DIGIT                       [0-9]
NUMBER                      [0-9]+

DURATION                    [0-9]{2}":"[0-9]{2}":"[0-9]{2}
TEMPERATURE                 {NUMBER}[CFK]
AMOUNT                      {NUMBER}[a-zA-Z]*

LOWERCASE_WORD              [a-z]+
CAPITALIZED_WORD            [A-Z][a-z]*

LOWERCASE_ID                {LOWERCASE_WORD}(_{LOWERCASE_WORD})*
CAPITALIZED_ID              {CAPITALIZED_WORD}(_{CAPITALIZED_WORD})*

%option noyywrap

%%
"Action"        { return T_DEF_ACTION; }
"Ingredient"    { return T_DEF_INGREDIENT; }
"Recipe"        { return T_DEF_RECIPE; }

"{"             { return T_LBRACE; }
"}"             { return T_RBRACE; }
"["             { return T_LBRACKET; }
"]"             { return T_RBRACKET; }
"("             { return T_LPAREN; }
")"             { return T_RPAREN; }
":"             { return T_COLON; }
","             { return T_COMMA; }
"."             { return T_DOT; }

"$I"            { return T_PARAM_INGREDIENT; }
"$E"            { return T_PARAM_EQUIPMENT; }
"$D"            { return T_PARAM_DURATION; }
"$C"            { return T_PARAM_CONDITION; }
"$T"            { return T_PARAM_TEMPERATURE; }

"in"            { return T_PARAM_IN; }
"with"          { return T_PARAM_WITH; }
"on"            { return T_PARAM_ON; }
"to"            { return T_PARAM_TO; }
"for"           { return T_PARAM_FOR; }
"until"         { return T_PARAM_UNTIL; }


"Ingredients"   { return T_RECIPE_INGREDIENTS; }
"Steps"         { return T_RECIPE_STEPS; }


{LOWERCASE_ID}          { yylval.sval = new std::string(yytext); return T_ID_LOWERCASE; }
{CAPITALIZED_ID}        { yylval.sval = new std::string(yytext); return T_ID_CAPITALIZED; }

{DURATION}              { yylval.sval = new std::string(yytext); return T_DURATION; }
{TEMPERATURE}           { yylval.sval = new std::string(yytext); return T_TEMPERATURE; }
{AMOUNT}                { yylval.sval = new std::string(yytext); return T_AMOUNT; }

[ \t]+                  ; /* Skip whitespace */
[\n]                    { yylineno++; }
.                       { std::cerr << "Lexer Error: Unexpected character " << *yytext << std::endl; }
%%