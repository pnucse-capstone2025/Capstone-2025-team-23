#ifndef RECIPE_HPP
#define RECIPE_HPP

#include <unordered_set>

#include "error.hpp"
#include "action.hpp"
#include "ingredient.hpp"


struct Step {
    std::string command;
    std::vector<std::string> target;
    std::map<std::string, ActionParameter> parameters;

    bool to_action(Action& action) const;
};


struct RecipeDefinition {
    std::string name;
    std::vector<Ingredient> ingredients;
    std::vector<Step> steps;
};

class Recipe {

public:
    Recipe(const RecipeDefinition& definition);

    void print();

    
private:
    std::string _name;
    std::vector<Ingredient> _ingredients;
    std::vector<Step> _steps;

    std::unordered_set<std::string> _equipments;
    std::unordered_set<std::string> _ingredientInIngredients;
    std::unordered_set<std::string> _ingredientInSteps;
    std::unordered_set<std::string> _relatedRecipes;
    std::vector<Action> _method;

    int _check_ingredients();
    int _check_steps();
    int _check_recipe_error();
    void _analyze_equipments();
    void _steps_to_method();

    void _print(int level);
    void _print_name(int level);
    void _print_equipments(int level);
    void _print_ingredients(int level);
    void _print_method(int level);
    void _print_related_recipes(int level);
    void _print_space(int level);
};

class RecipeManager {

public:
    bool is_defined(const std::string& name) const;
    void add_recipe(const RecipeDefinition& recipe);
    RecipeDefinition get_definition(const std::string& name) const;

private:
    std::map<std::string, RecipeDefinition> _recipes;
};

#endif // RECIPE_HPP