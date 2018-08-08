#include <iostream>
#include <fstream>
#include <utility>
#include <map>
#include <set>

#include "../include/makefile.h"
#include "../include/argument.h"
#include "../include/tools.h"

static const std::string GMAKE = ".gmake";
static const std::string OBJ = "obj";
static const std::vector<std::string> EXT_SRC = {".cpp", ".c++", ".cxx", ".cp", ".cc", ".c"};

bool getMainDependencies(const std::string& filename,
        const std::vector<std::string>& rootFolder,
        const std::map<std::string, std::set<std::string> >& hdrSrcMap,
        std::vector<std::pair<std::string, std::string> >& objs,
        std::vector<std::string>& deps){

    std::vector<std::string> headers;
    if(!readFileDeepDependencies(filename, headers)){
        return false;
    }

    headers.push_back(filename);

    for(const std::string& hdr : headers){
        for(const std::string root : rootFolder){
            std::string rootCan = fs::canonical(root);
            if(startWith(hdr, rootCan)){
                std::string stem = fs::path(hdr).stem();
                std::set<std::string> src = hdrSrcMap.find(hdr)!=hdrSrcMap.end()?hdrSrcMap.at(hdr):std::set<std::string>();
                if(src.find(stem)!=src.end()){
                    std::string filename = stem+".o";
                    objs.push_back(std::make_pair(root, OBJ+"/"+filename));
                }
                else{
                    deps.push_back(root/fs::relative(hdr, root));
                }
            }
        }
    }

    return true;
}

std::vector<std::string> getObjs(const std::vector<fs::path>& src, const std::string& folder){
    std::vector<std::string> objs;
    for(const fs::path& p : src){
        objs.push_back("$("+folder+")/"+p.filename().stem().string()+".o");
    }
    return objs;
}

bool processCurrentPath(){
    // read options in .gmake file
    GmakeOptions gmake;
    if(!readGmake(GMAKE, gmake)){
        return false;
    }

    // get data from .gmake
    std::vector<std::string> folders = gmake.folders;
    std::map<std::string, std::string> options = gmake.options;
    fs::path root = fs::current_path();

    // main() files
    std::vector<std::string> mainFiles, mainNames;
    bool isMain;

    // header / sources map
    std::map<std::string, std::set<std::string> > headerSourcesMap;

    /////////////////////////////////////////
    //////////////SubMakefiles///////////////
    /////////////////////////////////////////
    for(const std::string& folder : folders){
        Makefile makefile;
        makefile.addVar("CXX", options["compiler"]);
        makefile.addVar("ODIR", OBJ);
        makefile.addVar("CXXFLAGS", options["flags"]);

        // move to folder
        fs::path folderPath = fs::canonical(folder);
        fs::current_path(folderPath);

        // get files in current folder
        std::vector<fs::path> files = listdir(".");
        std::vector<fs::path> sources = filterPath(files, EXT_SRC);

        // first command (all)
        std::vector<std::string> objs = getObjs(sources, "ODIR");
        makefile.addArray("OBJS", objs);
        makefile.addRule("all", {"$(ODIR)", "$(OBJS)"}, {});
        makefile.addPhony("all");

        // compile *.cpp
        for(const fs::path& file : sources){
            std::string filename = file.filename();

            // enter parent folder of file
            fs::path subFolderPath = fs::canonical(file).parent_path();
            fs::current_path(subFolderPath);

            // get all includes from file
            std::vector<std::string> dependencies = {filename};
            if(!readFileDependencies(filename, dependencies, &isMain)){
                return false;
            }

            if(isMain){
                fs::path p(filename);
                mainFiles.push_back(fs::canonical(p));
                mainNames.push_back(p.stem());
                options[filename] = p.stem();
            }

            // fill header / sources map
            // set relative path to absolute path
            for(std::string& dep : dependencies){
                headerSourcesMap[fs::canonical(dep)].insert(fs::path(filename).stem());
                dep = fs::relative(fs::canonical(dep), folderPath);
            }

            // write rule
            std::string obj = file.stem().string()+".o";
            std::string compile = "$(CXX) -c $< -o $@ $(CXXFLAGS)";
            makefile.addRule("$(ODIR)/"+obj, dependencies, {compile});

            // go back to folder
            fs::current_path(folderPath);
        }

        // mkdir and rm obj/
        makefile.addRule("$(ODIR)", {}, {"if [ ! -d $(ODIR) ]; then mkdir $(ODIR); fi"});
        makefile.addRule("clean", {}, {"if [ -d $(ODIR) ]; then rm $(ODIR) -r; fi"});
        makefile.addPhony("clean");

        // create makefile
        if(!writeMakefile(makefile, folderPath))
            return false;

        // go back to root directory
        fs::current_path(root);
    }

    /////////////////////////////////////////
    ////////////////Makefile/////////////////
    /////////////////////////////////////////
    Makefile makefile;
    makefile.addVar("CXX", options["compiler"]);
    makefile.addVar("BIN", options["output"]);
    makefile.addVar("LIBS", options["libs"]);

    // entry command
    makefile.addRule("all", mainNames, {});
    makefile.addPhony("all");

    // entry for each main()
    for(const std::string& m : mainFiles){
        std::vector<std::pair<std::string, std::string> > objs;
        std::vector<std::string> deps, deps2, actions;
        if(!getMainDependencies(m, folders, headerSourcesMap, objs, deps)){
            return false;
        }

        // dependent files
        deps2.push_back("$(BIN)");
        deps2.insert(deps2.end(), deps.begin(), deps.end());

        std::map<std::string, std::vector<std::string> > submakeParam;
        for(const auto& o : objs){
            std::string root = o.first;
            std::string obj = o.second;
            submakeParam[root].push_back(obj);
        }

        for(const auto& p : submakeParam){
            std::stringstream ss;
            ss << "$(MAKE) -C " << p.first << " " << OBJ << " ";
            for(const std::string& s : p.second){
                ss << s << " ";
            }
            actions.push_back(ss.str());
        }

        // object to compile
        std::string maincpp = fs::path(m).filename();
        std::string main = fs::path(m).filename().stem();
        std::string exec = options.find(maincpp)!=options.end()?options[maincpp]:options["executable"];
        std::string objs_m = "OBJS_"+toUpper(main);
        std::vector<std::string> objs_compile;
        for(const auto& o : objs){
            objs_compile.push_back(fs::path(o.first)/o.second);
        }
        makefile.addArray(objs_m, objs_compile);

        actions.push_back("$(CXX) -o $(BIN)/"+exec+" $("+objs_m+") $(LIBS)");
        makefile.addRule(main, deps2, actions);
        makefile.addPhony(main);
    }

    // mkdir bin
    makefile.addRule("$(BIN)", {}, {"if [ ! -d $(BIN) ]; then mkdir $(BIN); fi"});

    // clean
    std::vector<std::string> cleanSub;
    for(const std::string& folder : folders){
        cleanSub.push_back("$(MAKE) -C "+folder+" clean");
    }
    cleanSub.push_back("if [ -d $(BIN) ]; then rm $(BIN) -r; fi");
    makefile.addRule("clean", {}, cleanSub);
    makefile.addPhony("clean");

    // create main makefile
    return writeMakefile(makefile, root);
}

bool initGmake(){
    std::ofstream file(GMAKE);
    if(file){
        file << "compiler = g++" << std::endl;
        file << "flags = -std=c++11" << std::endl << std::endl;
        file << "# set the name of the executable generated with main.cpp" << std::endl;
        file << "main.cpp = main" << std::endl << std::endl;
        file << "# add below the folders where should be generated makefiles..." << std::endl;
        file.close();
        return true;
    }
    return false;
}

int main(int argc, char const *argv[]) {
    Argument arg(argc, argv);
    if(arg.has("help")){
        std::cout << GMAKE << " file should be ceated." << std::endl;
        std::cout << "Use 'gmake init' to create a template." << std::endl;
    }
    else if(arg.has("init")){
        if(initGmake()){
            std::cout << "'" << GMAKE << "' created." << std::endl;
        }
        else{
            std::cout << "Could not create '" << GMAKE << "'." << std::endl;
        }
    }
    else {
        if(fs::exists(GMAKE)){
            if(processCurrentPath()){
                std::cout << "Done." << std::endl;
            }
            else{
                std::cout << "Could not write Makefile." << std::endl;
            }
        }
        else{
            std::cout << "Could not find '" << GMAKE << "' file." << std::endl;
            std::cout << "Use 'gmake init' to create a template." << std::endl;
        }
    }
    return 0;
}
