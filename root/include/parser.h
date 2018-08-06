#ifndef PARSER
#define PARSER

#include <string>

bool hasArg(const std::string& arg, int argc, char const *argv[]);
std::string getArg(const std::string& arg, int argc, char const *argv[]);

#endif
