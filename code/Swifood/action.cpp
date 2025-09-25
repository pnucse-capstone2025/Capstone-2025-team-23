#include "action.hpp"
#include "globals.hpp"
#include "swifood.hpp"
#include <vector>


std::string Action::to_string() const
{
    std::string ret;

    ret += name;
    ret[0] = toupper(ret[0]);
    for (auto& c: ret) {
        if (c == '_') {
            c = ' ';
        }
    }
    ret += " ";

    ret += vector_to_string(target);
    ret += " ";

    if (parameters.find("to") != parameters.end()) {
        ret += "to ";
        ret += vector_to_string(parameters.at("to").value);
        ret += " ";
    }
    if (parameters.find("in") != parameters.end()) {
        ret += "in ";
        ret += vector_to_string(parameters.at("in").value);
        ret += " ";
    }
    if (parameters.find("with") != parameters.end()) {
        ret += "with ";
        ret += vector_to_string(parameters.at("with").value);
        ret += " ";
    }
    if (parameters.find("on") != parameters.end()) {
        ret += "on ";
        ret += vector_to_string(parameters.at("on").value);
        ret += " ";
    }
    if (parameters.find("for") != parameters.end()) {
        ret += "for ";
        ret += vector_to_string(parameters.at("for").value);
        ret += " ";
    }
    if (parameters.find("until") != parameters.end()) {
        ret += "until ";
        ret += vector_to_string(parameters.at("until").value);
        ret += " ";
    }

    if (ret[ret.size() - 1] == ' ') {
        ret[ret.size() - 1] = '.';
    } else {
        ret += ".";
    }


    return ret;
}

bool ActionManager::is_defined(const std::string& name) const
{
    if (_actions.find(name) != _actions.end()) {
        return true;
    } else {
        return false;
    }
}

std::string ActionManager::get_target_type(const std::string& name) const
{
    if (is_defined(name)) {
        return _actions.at(name).targetType;
    } else {
        return "NOT DEFINED";
    }
}

void ActionManager::add_action(const ActionDefinition& action)
{
    if (is_defined(action.name)) {
        ErrorHandler::get_instance().report("Action", action.name, "is already defined.");
        return;
    }

    _actions[action.name] = action;
}

bool ActionManager::check_target(const Action& action) const
{
    if (!is_defined(action.name)) {
        ErrorHandler::get_instance().report("Action", action.name, "is not defined.");
        return false;
    } 
    
    if (_actions.at(action.name).targetType == "$I") {
        for (const auto& target: action.target) {
            if (!g_swifood->ingredientManager.is_defined(target) && !g_swifood->recipeManager.is_defined(target)) {
                return false;
            }
        }
    }


    return true;
}

bool ActionManager::check_required_parameters(const Action& action) const
{
    if (!is_defined(action.name)) {
        ErrorHandler::get_instance().report("Action", action.name, "is not defined.");
        return false;
    }

    for (const auto& check: _actions.at(action.name).requiredParameters) {
        if (action.parameters.find(check.first) == action.parameters.end()) {
            return false;
        }
    }


    return true;
}

bool ActionManager::check_xor_parameters(const Action& action) const
{
    if (!is_defined(action.name)) {
        ErrorHandler::get_instance().report("Action", action.name, "is not defined.");
        return false;
    }

    for (const auto& xorParameter: _actions.at(action.name).xorParameters) {
        for (const auto& name: xorParameter.nameList) {

        }
    }


    return true;
}

std::vector<std::string> ActionManager::get_required_parameters(const Action& action) const
{
    if (!is_defined(action.name)) {
        ErrorHandler::get_instance().report("Action", action.name, "is not defined.");
        return std::vector<std::string>();
    }

    std::vector<std::string> parameters;
    for (const auto& parameter: _actions.at(action.name).requiredParameters) {
        parameters.push_back(parameter.first);
    }


    return parameters;
}

void ActionManager::fill_default_parameters(Action& action) const
{
    if (!is_defined(action.name)) {
        ErrorHandler::get_instance().report("Action", action.name, "is not defined.");
        return;
    }

    for (const auto& parameter: _actions.at(action.name).defaultParameters) {
        if (action.parameters.find(parameter.first) == action.parameters.end()) {
            action.parameters[parameter.first] = parameter.second;
        }
    }

    std::vector<std::string> eraseList;
    for (auto& parameter: action.parameters) {
        for (auto& x: parameter.second.value) {
            if (x[0] == '$') {
                eraseList.push_back(parameter.first);
            }
        }
    }
    for (const auto& erase: eraseList) {
        action.parameters.erase(erase);
    }
}