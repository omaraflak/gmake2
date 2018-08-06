#include "../include/argument.h"

Argument::Argument(int argc, char const* argv[]){
    for(int i=0 ; i<argc ; i++){
        set.insert(argv[i]);
    }
}

bool Argument::has(const std::string& arg){
    return set.find(arg)!=set.end();
}
