#include "../include/tools.h"

std::vector<fs::path> listdir(const std::string& dir){
    fs::path p(dir);
    fs::directory_iterator end_itr;
    std::vector<fs::path> list;

    for (fs::directory_iterator itr(p); itr != end_itr; ++itr){
        if (is_regular_file(itr->path())) {
            list.push_back(itr->path());
        }
        else{
            std::vector<fs::path> subdir = listdir(itr->path().string());
            list.insert(list.end(), subdir.begin(), subdir.end());
        }
    }

    return list;
}

std::vector<fs::path> filterPath(const std::vector<fs::path>& paths, const std::vector<std::string>& ext){
    std::vector<fs::path> path;
    for(const fs::path& p : paths){
        if(contains(ext, p.extension().string())){
            path.push_back(p);
        }
    }
    return path;
}

bool readFileDependencies(const std::string& filename, std::vector<std::string> &deps, bool* isMain){
    std::ifstream file(filename.c_str());
    if(file){
        if(isMain!=NULL){
            *isMain = false;
        }

        std::string line;
        while(!file.eof()){
            getline(file, line);
            eraseAll(line, " ");
            if(startWith(line, INCLUDE_STMT)){
                int ld = line.find(INCLUDE_L_DLMTR);
                int rd = line.find(INCLUDE_R_DLMTR, ld+1);
                if(ld!=-1 && rd!=-1 && ld!=rd){
                    std::string dependency = line.substr(ld+1, rd-ld-1);
                    deps.push_back(dependency);
                }
            }
            else if(startWith(line, MAIN_FUNCT) && isMain!=NULL){
                *isMain = true;
            }
        }
        file.close();
        return true;
    }
    return false;
}

bool readFileDeepDependencies_(const std::string& filename, std::vector<std::string> &deps){
    std::vector<std::string> d;
    if(!readFileDependencies(filename, d))
        return false;

    // change directory if needed
    std::string parent = fs::path(filename).parent_path().string();
    if(parent!=""){
        fs::current_path(parent);
    }

    // relative to canonical & add new dependencies
    for(std::string& path : d){
        path = fs::canonical(path);
        if(!contains(deps, path)){
            deps.push_back(path);
        }
    }

    // make it recursive...
    for(const std::string& file : d){
        if(!readFileDeepDependencies(file, deps))
            return false;
    }

    return true;
}

bool readFileDeepDependencies(const std::string& filename, std::vector<std::string> &deps){
    fs::path root = fs::current_path();
    bool result = readFileDeepDependencies_(filename, deps);
    fs::current_path(root);
    return result;
}

bool writeMakefile(const Makefile& makefile, const std::string& folder){
    std::string filepath = fs::path(folder)/fs::path("Makefile");
    std::ofstream out(filepath.c_str());
    if(out){
        out << makefile.build() << std::endl;
        out.close();
        return true;
    }
    return false;
}

bool readGmake(const std::string& filepath, GmakeOptions& gmake){
    std::string line;
    std::ifstream file(filepath.c_str());
    if(file){
        while(!file.eof()){
            std::getline(file, line);
            if(line!=""){
                line = trim(line);
                if(line[0]!='#'){
                    int sep = line.find(GmakeOptions::separator);
                    if(sep==-1){
                        gmake.folders.push_back(trim(line));
                    } else{
                        gmake.options[trim(line.substr(0, sep))] = trim(line.substr(sep+1));
                    }
                }
            }
        }
        file.close();
        return true;
    }
    return false;
}

bool startWith(const std::string& str, const std::string& sub){
    return str.find(sub)==0;
}

bool endWith(const std::string& str, const std::string& sub){
    return (str.size() >= sub.size() && str.compare(str.size() - sub.size(), sub.size(), sub) == 0);
}

void eraseAll(std::string &str, const std::string& sub){
    int pos = 0;
    int length = sub.size();
    while((pos = str.find(sub))!=-1){
        str.replace(pos, length, "");
    }
}

std::string toUpper(const std::string& str){
    std::locale loc;
    std::stringstream ss;
    for (std::string::size_type i=0; i<str.length(); ++i)
        ss << std::toupper(str[i],loc);
    return ss.str();
}

std::string trim(const std::string& str){
    int start = str.find_first_not_of(' ');
    int end = str.find_last_not_of(' ');
    return str.substr(start, end-start+1);
}
