#ifndef GMAKE_OPTIONS
#define GMAKE_OPTIONS

#include <vector>
#include <string>
#include <map>

struct GmakeOptions {
    std::vector<std::string> folders;
    std::map<std::string, std::string> options;
    static const char separator = '=';

    GmakeOptions(){
        options["compiler"] = "g++";
        options["flags"] = "-Wall -Wextra -std=c++11";
        options["libs"] = "";
        options["executable"] = "main";
        options["output"] = "bin";
    }
};

#endif
