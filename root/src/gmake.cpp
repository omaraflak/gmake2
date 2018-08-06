#include <iostream>
#include <sstream>

#include "../include/makefile.h"
#include "../include/parser.h"
#include "../include/tools.h"

static const std::vector<std::string> EXT_SRC = {".cpp", ".c++", ".cxx", ".cp", ".cc", ".c"};

std::vector<std::string> getObjs(const std::vector<fs::path>& src, const std::string& folder){
    std::vector<std::string> objs;
    for(const fs::path& p : src){
        objs.push_back("$("+folder+")/"+p.filename().stem().string()+".o");
    }
    return objs;
}

int main() {
    GmakeOptions gmake;
    if(readGmake(".gmake", gmake)){
        std::vector<std::string> folders = gmake.folders;
        std::map<std::string, std::string> options = gmake.options;
        fs::path root = fs::current_path();

        // generate submakefiles
        for(const std::string& folder : folders){
            Makefile makefile;
            makefile.addVar("CC", options["compiler"]);
            makefile.addVar("ODIR", "obj");
            makefile.addVar("CXXFLAGS", options["flags"]);

            // move to folder
            fs::path folderPath = fs::canonical(folder);
            fs::current_path(folderPath);

            // get files in current folder
            std::vector<fs::path> files = listdir(".");
            std::vector<fs::path> paths = filterPath(files, EXT_SRC);

            // first command (all)
            std::vector<std::string> objs = getObjs(paths, "ODIR");
            makefile.addArray("OBJS", objs);
            makefile.addRule("all", {"$(ODIR)", "$(OBJS)"}, {});

            // compile *.cpp
            for(const fs::path& file : paths){
                std::string filename = file.filename();

                // enter parent folder of file
                fs::path subFolderPath = fs::canonical(file).parent_path();
                fs::current_path(subFolderPath);

                // get all includes from file
                std::vector<std::string> dependencies = {filename};
                readFileDependencies(filename, dependencies);

                // set relative path to absolute path
                for(std::string& dep : dependencies){
                    dep = fs::relative(fs::canonical(dep), folderPath);
                }

                // write rule
                std::string obj = file.stem().string()+".o";
                std::string compile = "$(CC) -c $< -o $@ $(CXXFLAGS)";
                makefile.addRule("$(ODIR)/"+obj, dependencies, {compile});

                // go back to folder
                fs::current_path(folderPath);
            }

            // mkdir and rm obj/
            makefile.addRule("$(ODIR)", {}, {"if [ ! -d $(ODIR) ]; then mkdir $(ODIR); fi"});
            makefile.addPhony("clean", {"if [ -d $(ODIR) ]; then rm $(ODIR) -r; fi"});

            // create makefile
            writeMakefile(makefile, folderPath);

            // go back to root directory
            fs::current_path(root);
        }

        // generate main makefile
        Makefile makefile;
        makefile.addVar("CC", options["compiler"]);
        makefile.addVar("BIN", options["output"]);
        makefile.addVar("CXXFLAGS", options["flags"]);
        makefile.addVar("PROG", options["executable"]);
        makefile.addVar("SUBMAKEFILES", "submakefiles");

        // build folders related lines
        std::vector<std::string> objs, makeSub, cleanSub;
        for(const std::string& folder : folders){
            objs.push_back("$(wildcard "+folder+"/obj/*.o)");
            makeSub.push_back("$(MAKE) -C "+folder);
            cleanSub.push_back("$(MAKE) -C "+folder+" clean");
        }
        cleanSub.push_back("if [ -d $(BIN) ]; then rm $(BIN) -r; fi");

        // write all
        makefile.addArray("OBJS", objs);
        makefile.addRule("$(PROG)", {"$(BIN)", "$(SUBMAKEFILES)"}, {"$(CC) -o $(BIN)/$@ $(OBJS) $(CXXFLAGS)"});
        makefile.addRule("$(BIN)", {}, {"if [ ! -d $(BIN) ]; then mkdir $(BIN); fi"});
        makefile.addPhony("$(SUBMAKEFILES)", makeSub);
        makefile.addPhony("clean", cleanSub);

        // create main makefile
        writeMakefile(makefile, root);
    }
    else{
        std::cout << "no .gmake file found." << std::endl;
    }
    return 0;
}
