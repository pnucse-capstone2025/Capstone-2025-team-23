%{
#include "swifood.hpp"
#include "globals.hpp"
#include "recipe.hpp"
#include "action.hpp"
#include <string>
#include <vector>
#include <map>

extern "C" int yylex();
extern int yylineno;

void yyerror(const char *s);
%}

%union {
    std::string* sval;
    std::vector<std::string>* vec_sval;
    Step* step;
    RecipeDefinition* recipe;
    std::map<std::string, ActionParameter>* param_map;
    std::vector<Step*>* step_list;
    std::vector<Ingredient>* ingredient_list;
    Ingredient* ingredient;

    ActionDefinition* action_def;
    ActionParameter* action_param;
    std::vector<ActionParameter*>* param_list;
    std::vector<ActionParameter*>* action_param_list;
    std::string* param_type_sval;
}

%token <sval> T_ID_LOWERCASE T_ID_CAPITALIZED
%token <sval> T_DURATION T_TEMPERATURE T_AMOUNT
%token T_DEF_ACTION T_DEF_INGREDIENT T_DEF_RECIPE
%token T_LBRACE T_RBRACE T_LPAREN T_RPAREN T_LBRACKET T_RBRACKET
%token T_COLON T_COMMA T_DOT T_PIPE
%token T_PARAM_INGREDIENT T_PARAM_EQUIPMENT T_PARAM_DURATION T_PARAM_CONDITION T_PARAM_TEMPERATURE
%token T_PARAM_IN T_PARAM_WITH T_PARAM_ON T_PARAM_TO T_PARAM_FOR T_PARAM_UNTIL
%token T_RECIPE_INGREDIENTS T_RECIPE_STEPS

%type <sval> capitalized_identifier lowercase_identifier general_identifier action_param_key parameter_key
%type <vec_sval> capitalized_identifier_list capitalized_identifiers lowercase_identifier_list lowercase_identifiers general_identifiers general_identifier_list step_targets
%type <recipe> recipe_definition
%type <step> step
%type <param_map> optional_parameter_list parameter_list
%type <step_list> step_list
%type <ingredient_list> recipe_ingredient_list
%type <ingredient> recipe_ingredient

%type <action_def> action_definition
%type <param_type_sval> action_target_type parameter_value_type
%type <action_param_list> action_body param_list_items
%type <action_param> any_param default_param required_param

%%

program:
    definitions
;

definitions:
    /* empty */
    | definitions definition
;

definition:
    ingredient_definition
    | recipe_definition {
        g_swifood->recipeManager.add_recipe(*$1);
        delete $1;
    }
    | action_definition {
        g_swifood->actionManager.add_action(*$1);
        delete $1;
    }
;

ingredient_definition:
    T_DEF_INGREDIENT T_ID_CAPITALIZED T_LBRACE capitalized_identifiers T_RBRACE
    {
        g_swifood->ingredientManager.add_ingredients(*$2, *$4);
        delete $2; delete $4;
    }
    | T_DEF_INGREDIENT T_LBRACE capitalized_identifiers T_RBRACE
    {
        g_swifood->ingredientManager.add_ingredients("", *$3);
        delete $3;
    }
;

action_definition:
    T_DEF_ACTION T_ID_LOWERCASE action_target_type T_LBRACE action_body T_RBRACE
    {
        $$ = new ActionDefinition();
        $$->name = *$2;
        $$->targetType = *$3;

        if ($5 != nullptr) {
            for (auto p : *$5) {
                $$->defaultParameters[p->name] = *p;
                delete p;
            }
            delete $5;
        }
        delete $2; delete $3;
    }
;

action_target_type:
    T_PARAM_INGREDIENT  { $$ = new std::string("$I"); }
    | T_PARAM_EQUIPMENT { $$ = new std::string("$E"); }
;

action_body:
    /* empty */       { $$ = nullptr; }
    | param_list_items { $$ = $1; }
;

param_list_items:
    any_param { $$ = new std::vector<ActionParameter*>{$1}; }
    | param_list_items T_COMMA any_param { $1->push_back($3); $$ = $1; }
;

any_param:
    default_param { $$ = $1; }
    | required_param { $$ = $1; }
;

action_param_key:
    T_PARAM_IN      { $$ = new std::string("in"); }
    | T_PARAM_WITH  { $$ = new std::string("with"); }
    | T_PARAM_ON    { $$ = new std::string("on"); }
    | T_PARAM_TO    { $$ = new std::string("to"); }
    | T_PARAM_FOR   { $$ = new std::string("for"); }
    | T_PARAM_UNTIL { $$ = new std::string("until"); }
;

default_param:
    action_param_key T_COLON general_identifier_list
    {
        $$ = new ActionParameter();
        $$->name = *$1;
        $$->value = *$3;
        delete $1;
    }
;

required_param:
    action_param_key T_COLON parameter_value_type
    {
        $$ = new ActionParameter();
        $$->name = *$1;
        $$->value.push_back(*$3);
        delete $1;
    }
;

parameter_value_type:
    T_PARAM_INGREDIENT    { $$ = new std::string("$I"); }
    | T_PARAM_EQUIPMENT   { $$ = new std::string("$E"); }
    | T_PARAM_DURATION    { $$ = new std::string("$D"); }
    | T_PARAM_CONDITION   { $$ = new std::string("$C"); }
    | T_PARAM_TEMPERATURE { $$ = new std::string("$T"); }
;

recipe_definition:
    T_DEF_RECIPE T_ID_CAPITALIZED T_LBRACE
        T_RECIPE_INGREDIENTS T_COLON recipe_ingredient_list T_DOT
        T_RECIPE_STEPS T_COLON step_list
    T_RBRACE
    {
        $$ = new RecipeDefinition();
        $$->name = *$2;
        $$->ingredients = *$6;
        if ($10 != nullptr) {
            for(auto s : *$10) {
                $$->steps.push_back(*s);
                delete s; 
            }
            delete $10;
        }
        delete $2; delete $6;
    }
;

recipe_ingredient_list:
    recipe_ingredient { $$ = new std::vector<Ingredient>{*$1}; delete $1; }
    | recipe_ingredient_list T_COMMA recipe_ingredient { $1->push_back(*$3); $$ = $1; delete $3; }
;

recipe_ingredient:
    T_ID_CAPITALIZED T_AMOUNT { $$ = new Ingredient{*$1, *$2}; delete $1; delete $2; }
    | T_ID_CAPITALIZED        { $$ = new Ingredient{*$1, ""}; delete $1; }
;

step_list:
    step { $$ = new std::vector<Step*>{$1}; }
    | step_list step { $1->push_back($2); $$ = $1; }
;

step:
    T_ID_LOWERCASE step_targets optional_parameter_list T_DOT
    {
        $$ = new Step();
        $$->command = *$1;
        $$->target = *$2;
        if ($3 != nullptr) {
            $$->parameters = *$3;
            delete $3;
        }
        delete $1; delete $2;
    }
;

step_targets:
    general_identifier { $$ = new std::vector<std::string>{*$1}; delete $1; }
    | general_identifier_list { $$ = $1; }
;

general_identifier:
    T_ID_CAPITALIZED        { $$ = $1; }
    | T_ID_LOWERCASE        { $$ = $1; }
    | T_AMOUNT              { $$ = $1; }
    | T_DURATION            { $$ = $1; }
    | T_TEMPERATURE         { $$ = $1; }
;

general_identifiers:
    general_identifier { $$ = new std::vector<std::string>{*$1}; delete $1; }
    | general_identifiers T_COMMA general_identifier { $1->push_back(*$3); $$ = $1; delete $3; }
;

general_identifier_list:
    general_identifier { $$ = new std::vector<std::string>{*$1}; delete $1; }
    | T_LBRACKET general_identifiers T_RBRACKET { $$ = $2; }
;

capitalized_identifier:
    T_ID_CAPITALIZED { $$ = $1; }
;

capitalized_identifiers:
    capitalized_identifier { $$ = new std::vector<std::string>{*$1}; delete $1; }
    | capitalized_identifiers T_COMMA T_ID_CAPITALIZED { $1->push_back(*$3); $$ = $1; delete $3; }
;

capitalized_identifier_list:
    capitalized_identifier { $$ = new std::vector<std::string>{*$1}; delete $1; }
    | T_LBRACKET capitalized_identifiers T_RBRACKET { $$ = $2; }
;

lowercase_identifier:
    T_ID_LOWERCASE { $$ = $1; }
;

lowercase_identifiers:
    lowercase_identifier { $$ = new std::vector<std::string>{*$1}; delete $1; }
    | lowercase_identifiers T_COMMA T_ID_LOWERCASE { $1->push_back(*$3); $$ = $1; delete $3; }
;

lowercase_identifier_list:
    lowercase_identifier { $$ = new std::vector<std::string>{*$1}; delete $1; }
    | T_LBRACKET lowercase_identifiers T_RBRACKET { $$ = $2; }
;

optional_parameter_list:
    /* empty */ { $$ = nullptr; }
    | T_LPAREN parameter_list T_RPAREN { $$ = $2; }
;

parameter_key:
    T_PARAM_IN         { $$ = new std::string("in"); }
    | T_PARAM_WITH     { $$ = new std::string("with"); }
    | T_PARAM_ON       { $$ = new std::string("on"); }
    | T_PARAM_TO       { $$ = new std::string("to"); }
    | T_PARAM_FOR      { $$ = new std::string("for"); }
    | T_PARAM_UNTIL    { $$ = new std::string("until"); }
    | T_ID_LOWERCASE   { $$ = $1; }
;

parameter_list:
    parameter_key T_COLON general_identifier_list
    {
        $$ = new std::map<std::string, ActionParameter>();
        (*$$)[*$1].name = *$1;
        (*$$)[*$1].value = *$3;
        delete $1; delete $3;
    }
    | parameter_list T_COMMA parameter_key T_COLON general_identifier_list
    {
        $$ = $1;
        (*$$)[*$3].name = *$3;
        (*$$)[*$3].value = *$5;
        delete $3; delete $5;
    }
;

%%

void yyerror(const char *s) {
    ErrorHandler::get_instance().report("File", g_currentFileName, "has error in line " + std::to_string(yylineno) + ", near '" + yytext + "'" + " (" + s + ").");
}