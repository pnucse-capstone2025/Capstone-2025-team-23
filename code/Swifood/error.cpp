#include "error.hpp"


ErrorHandler& ErrorHandler::get_instance()
{
    static ErrorHandler instance;

    return instance;
}

void ErrorHandler::report(const std::string& type, const std::string& target, const std::string& msg)
{
    Error error = {.type = type, .target = target, .msg = msg};

    _errorList.push_back(error);
}

bool ErrorHandler::is_error_exist() const
{
    if (_errorList.size() > 0) {
        return true;
    } else {
        return false;
    }
}

void ErrorHandler::print_errors() const
{
    for (const auto& error: _errorList) {
        std::cerr << "Error: " << error.type + " " + "'" + error.target + "' " << error.msg << std::endl;
    }
    std::cerr << _errorList.size() << " errors are found." << std::endl;
}
