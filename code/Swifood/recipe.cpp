#include <cctype>

#include "recipe.hpp"
#include "error.hpp"
#include "globals.hpp"
#include "swifood.hpp"
#include "utils.hpp"


bool Step::to_action(Action& action) const
{
    if (!g_swifood->actionManager.is_defined(command)) {
        ErrorHandler::get_instance().report("Action", command, "is not defined.");
        return false;
    }

    action.name = command;
    action.target = target;
    action.parameters = parameters;

    if (!g_swifood->actionManager.check_target(action)) {
        ErrorHandler::get_instance().report("Action", command, "is using undefined target.");
        return false;
    }

    if (!g_swifood->actionManager.check_required_parameters(action)) {
        ErrorHandler::get_instance().report("Action", command, "is missing required parameters. [" + vector_to_string(g_swifood->actionManager.get_required_parameters(action)) + "]");
        return false;
    }

    if (!g_swifood->actionManager.check_xor_parameters(action)) {
        ErrorHandler::get_instance().report("Action", command, "has some issues with the XOR parameters.");
        return false;
    }

    g_swifood->actionManager.fill_default_parameters(action);


    return true;
}

Recipe::Recipe(const RecipeDefinition& definition)
{
    _name = definition.name;
    _ingredients = definition.ingredients;
    _steps = definition.steps;

    int checkSum =  _check_ingredients() + _check_steps() + _check_recipe_error();
    if (checkSum > 0) {
        ErrorHandler::get_instance().report("Recipe", _name, "has " + std::to_string(checkSum) + " problems.");
    }

    _steps_to_method();
    _analyze_equipments();
}

void Recipe::print()
{
    _print(0);
}

void Recipe::_print(int level)
{
    int checkSum = _check_ingredients() + _check_steps() + _check_recipe_error();

    if (checkSum == 0) {
        _print_name(level);
        std::cout << std::endl;

        _print_equipments(level);
        std::cout << std::endl;

        _print_ingredients(level);
        std::cout << std::endl;

        _print_method(level);
        std::cout << std::endl;

        _print_related_recipes(level);
        std::cout << std::endl;
    }
}

void Recipe::_print_name(int level)
{
    _print_space(level);
    std::cout << "[" << _name << "]" << std::endl;
}

void Recipe::_print_equipments(int level)
{
    _print_space(level);
    std::cout << "--- Equipments ---" << std::endl;

    for (const auto& equipment: _equipments) {
        _print_space(level);
        std::cout << "    " << equipment << std::endl;
    }
}

void Recipe::_print_ingredients(int level)
{
    _print_space(level);
    std::cout << "--- Ingredients ---" << std::endl;

    for (const auto& ingredient: _ingredients) {
        _print_space(level);
        std::cout << "    * " << ingredient.name << " " << ingredient.amount << std::endl;
    }
}

void Recipe::_print_method(int level)
{
    _print_space(level);
    std::cout << "--- Method ---" << std::endl;

    for (int i = 0; i < _method.size(); i++) {
        _print_space(level);
        std::cout << "    " << i + 1 << ". " << _method[i].to_string() << std::endl;
    }
}

void Recipe::_print_related_recipes(int level)
{
    if (_relatedRecipes.empty()) {
        return;
    }

    _print_space(level);
    std::cout << "    " << "--- Related recipes ---" << std::endl << std::endl;
    for (const auto& name: _relatedRecipes) {
        Recipe recipe(g_swifood->recipeManager.get_definition(name));

        recipe._print(level + 1);
        std::cout << std::endl;
    }
}

void Recipe::_print_space(int level)
{
    for (int i = 0; i < level; i++) {
        std::cout << "    ";
    }
}

int Recipe::_check_recipe_error()
{
    int checkSum = 0;

    if (_ingredientInIngredients != _ingredientInSteps) {
        std::string undeclaredIngredients = "[ ";
        std::string unusedIngredients = "[ ";
        for (const auto& ingredient: _ingredientInSteps) {
            if (_ingredientInIngredients.find(ingredient) == _ingredientInIngredients.end()) {
                undeclaredIngredients += ingredient + " ";
            }
        }
        undeclaredIngredients += "]";
        for (const auto& ingredient: _ingredientInIngredients) {
            if (_ingredientInSteps.find(ingredient) == _ingredientInSteps.end()) {
                unusedIngredients += ingredient + " ";
            }
        }
        unusedIngredients += "]";

        ErrorHandler::get_instance().report("Recipe", _name, "is not complete. Maybe some ingredients were not used. (List: " + unusedIngredients + "). Maybe some ingredients were not declared. (List: " + undeclaredIngredients + ")");
        checkSum++;
    }


    return checkSum;
}

int Recipe::_check_ingredients()
{
    int checkSum = 0;


    for (const auto& ingredient: _ingredients) {
        if (ingredient.name == _name)  {
            ErrorHandler::get_instance().report("Recipe", _name, "is using itself as ingredient.");
            checkSum++;
        }
        if (g_swifood->recipeManager.is_defined(ingredient.name)) {
            _relatedRecipes.insert(ingredient.name);
            _ingredientInIngredients.insert(ingredient.name);
        } else if (!g_swifood->ingredientManager.is_defined(ingredient.name)) {
            ErrorHandler::get_instance().report("Ingredient", ingredient.name, "is not defined.");
            checkSum++;
        } else  {
            _ingredientInIngredients.insert(ingredient.name);
        }
    }

    if (checkSum > 0) {
        ErrorHandler::get_instance().report("Recipe", _name, "has " + std::to_string(checkSum) + " problems in <Ingredients>.");
    }


    return checkSum;
}

int Recipe::_check_steps()
{
    int checkSum = 0;

    for (const auto& step: _steps) {
        if (!g_swifood->actionManager.is_defined(step.command)) {
            ErrorHandler::get_instance().report("Action", step.command, "is not defined.");
            checkSum++;
        } else if (g_swifood->actionManager.get_target_type(step.command) == "$I") {
            for (const auto& ingredient: step.target) {
                if (!g_swifood->ingredientManager.is_defined(ingredient) && !g_swifood->recipeManager.is_defined(ingredient)) {
                    ErrorHandler::get_instance().report("Ingredient", ingredient, "is not defined.");
                    checkSum++;
                }
                _ingredientInSteps.insert(ingredient);
            }
        }
    }

    if (checkSum > 0) {
        ErrorHandler::get_instance().report("Recipe", _name, "has " + std::to_string(checkSum) + " problems in <Steps>.");
    }

    return checkSum;
}

void Recipe::_analyze_equipments()
{
    for (const auto& step: _steps) {
        if (step.parameters.find("in") != step.parameters.end()) {
            for (const auto& equipment: step.parameters.at("in").value) {
                _equipments.insert(equipment);
            }
        }
        if (step.parameters.find("on") != step.parameters.end()) {
            for (const auto& equipment: step.parameters.at("on").value) {
                _equipments.insert(equipment);
            }
        }
        if (step.parameters.find("with") != step.parameters.end()) {
            for (const auto& equipment: step.parameters.at("with").value) {
                _equipments.insert(equipment);
            }
        }
    }
}

void Recipe::_steps_to_method()
{
    for (int i = 0; i < _steps.size(); i++) {
        Action action;

        if (!_steps[i].to_action(action)) {
            ErrorHandler::get_instance().report("Step", std::to_string(i + 1), "has some problems.");
        }

        _method.push_back(action);
    }
}

bool RecipeManager::is_defined(const std::string& name) const
{
    if (_recipes.find(name) != _recipes.end()) {
        return true;
    } else {
        return false;
    }
}

void RecipeManager::add_recipe(const RecipeDefinition& recipe)
{
    if (is_defined(recipe.name)) {
        ErrorHandler::get_instance().report("Recipe", recipe.name, "is already defined.");
        return;
    }

    _recipes[recipe.name] = recipe;
}

RecipeDefinition RecipeManager::get_definition(const std::string& name) const
{
    if (!is_defined(name)) {
        ErrorHandler::get_instance().report("Recipe", name, "is not defined.");
        return RecipeDefinition();
    }

    else {
        return _recipes.at(name);
    }
}