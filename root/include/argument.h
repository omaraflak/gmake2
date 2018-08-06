#ifndef ARGUMENT
#define ARGUMENT

#include <string>
#include <set>

class Argument {
    private:
        std::set<std::string> set;

    public:
        Argument(int argc, char const* argv[]);
        bool has(const std::string& arg);
};

#endif
