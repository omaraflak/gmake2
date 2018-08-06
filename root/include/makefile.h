#ifndef MAKEFILE
#define MAKEFILE

#include <string>
#include <sstream>
#include <vector>

class Makefile {
    private:
        std::stringstream varStream;
        std::stringstream arrayStream;
        std::stringstream ruleStream;
        std::stringstream phonyStream;

    public:
        Makefile();

        void addVar(const std::string& name, const std::string& value);
        void addArray(const std::string& name, const std::vector<std::string>& values);
        void addRule(const std::string& name, const std::vector<std::string>& dependencies, const std::vector<std::string>& actions);
        void addPhony(const std::string& name);
        std::string build() const;
};

#endif
