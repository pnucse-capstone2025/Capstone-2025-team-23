#include "ingredient.hpp"


void IngredientManager::add_ingredients(const std::string& category, const std::vector<std::string>& ingredients)
{
    for (const auto& ingredient: ingredients) {
        if (_ingredients.find(ingredient) != _ingredients.end()) {
            ErrorHandler::get_instance().report("Ingredient", ingredient, "is already defined.");
        } else {
            _ingredients.insert(ingredient);
            _ingredientToCategory[ingredient] = category;
            _categoryToIngredients[category].push_back(ingredient);
        }
    }
}

bool IngredientManager::is_defined(const std::string& name) const
{
    if (_ingredients.find(name) != _ingredients.end()) {
        return true;
    } else {
        return false;
    }
}

std::string IngredientManager::get_category(const std::string& name) const
{
    if (_ingredients.find(name) != _ingredients.end()) {
        return _ingredientToCategory.at(name);
    } else {
        ErrorHandler::get_instance().report("Ingredient", name, "is not defined.");
        return "NOT DEFINED";
    }
}
