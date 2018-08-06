#include "../include/makefile.h"

Makefile::Makefile(){}

void Makefile::addVar(const std::string& name, const std::string& value){
    varStream << name << " = " << value << std::endl;
}

void Makefile::addArray(const std::string& name, const std::vector<std::string>& values){
    arrayStream << name << " = ";
    for(const std::string& e : values){
        arrayStream << e << " ";
    }
    arrayStream << std::endl;
}

void Makefile::addRule(const std::string& name, const std::vector<std::string>& dependencies, const std::vector<std::string>& actions){
    ruleStream << name << " : ";
    for(const std::string& d : dependencies){
        ruleStream << d << " ";
    }
    ruleStream << std::endl;
    for(const std::string& a : actions){
        ruleStream << "\t" << a << std::endl;
    }
    ruleStream << std::endl;
}

void Makefile::addPhony(const std::string& name, const std::vector<std::string>& actions){
    phonyStream << ".PHONY : " << name << std::endl;
    phonyStream << name << " : " << std::endl;
    for(const std::string& a : actions){
        phonyStream << "\t" << a << std::endl;
    }
    phonyStream << std::endl;
}

std::string Makefile::build() const{
    std::stringstream ss;
    ss << varStream.str() << std::endl;
    ss << arrayStream.str() << std::endl;
    ss << ruleStream.str() << std::endl;
    ss << phonyStream.str();
    return ss.str();
}
