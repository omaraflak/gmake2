#include "../include/parser.h"

bool hasArg(const std::string& arg, int argc, char const *argv[]){
    for(int i=0 ; i<argc ; i++){
        if(argv[i]==arg){
            if(i<argc-1){
                return true;
            }
        }
    }
    return false;
}

std::string getArg(const std::string& arg, int argc, char const *argv[]){
    for(int i=0 ; i<argc ; i++){
        if(argv[i]==arg){
            return argv[i+1];
        }
    }
    return "";
}
