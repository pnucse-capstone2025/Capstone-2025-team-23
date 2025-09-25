#ifndef INGREDIENT_HPP
#define INGREDIENT_HPP

#include "error.hpp"


struct Ingredient {
    std::string name;
    std::string amount;
};

class IngredientManager {

public:
    void add_ingredients(const std::string& category, const std::vector<std::string>& ingredients);
    bool is_defined(const std::string& name) const;
    std::string get_category(const std::string& name) const;

private:
    std::unordered_set<std::string> _ingredients;
    std::map<std::string, std::string> _ingredientToCategory;
    std::map<std::string, std::vector<std::string>> _categoryToIngredients;
};

#endif // INGREDIENT_HPP