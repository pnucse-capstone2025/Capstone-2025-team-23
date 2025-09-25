#include "utils.hpp"


std::string vector_to_string(const std::vector<std::string>& vec)
{
    std::string ret;

    if (vec.size() > 1) {
        for (int i = 0; i < vec.size() - 1; i++) {
            ret += vec[i];
            ret += ", ";
        }
        ret += "and ";
        ret += vec[vec.size() - 1];
    } else {
        ret += vec[0];
    }


    return ret;
}