#ifndef SWIFOOD_HPP
#define SWIFOOD_HPP

#include "error.hpp"
#include "action.hpp"
#include "ingredient.hpp"
#include "recipe.hpp"


class SwifoodLibrary {

public:
    void load(const std::string& fileName);

    IngredientManager ingredientManager;
    ActionManager actionManager;
    RecipeManager recipeManager;

private:

};

#endif // SWIFOOD_HPP