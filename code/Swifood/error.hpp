#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>

#include "globals.hpp"


struct Error {
    std::string type;
    std::string target;
    std::string msg;
};

class ErrorHandler {

public:
    static ErrorHandler& get_instance();

    void report(const std::string& type, const std::string& target, const std::string& msg);
    bool is_error_exist() const;
    void print_errors() const;

    
private:
    std::vector<Error> _errorList;
};
