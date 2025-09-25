#ifndef ACTION_HPP
#define ACTION_HPP

#include "error.hpp"
#include "utils.hpp"


struct ActionParameter {
    std::string name;
    std::vector<std::string> value;
};

struct ActionXorParameter {
    std::vector<std::string> nameList;
    std::map<std::string, ActionParameter> parameters;
};

struct ActionDefinition {
    std::string name;
    std::string targetType;
    std::map<std::string, ActionParameter> defaultParameters;
    std::map<std::string, ActionParameter> requiredParameters;
    std::vector<ActionXorParameter> xorParameters;
};

struct Action {
    std::string name;
    std::vector<std::string> target;
    std::map<std::string, ActionParameter> parameters;

    std::string to_string() const;
};

class ActionManager {

public:
    bool is_defined(const std::string& name) const;
    std::string get_target_type(const std::string& name) const;

    void add_action(const ActionDefinition& action);

    bool check_target(const Action& action) const;
    bool check_required_parameters(const Action& action) const;
    bool check_xor_parameters(const Action& action) const;
    
    std::vector<std::string> get_required_parameters(const Action& action) const;
    void fill_default_parameters(Action& action) const;


private:
    std::map<std::string, ActionDefinition> _actions;


};

#endif // ACTION_HPP