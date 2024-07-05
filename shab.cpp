#define SHAB_IMPLEMENTATION
#include "shab.hpp"

// g++ -std=c++23 -o shab shab.cpp

int main(int argc, char **argv) {
    rebuild_yourself(argc, argv, __FILE__);

    std::string output = "main.js";
    std::string source = "main.cpp";

    std::vector<std::string> exported_functions = {"_render"};

    if (shab_needs_rebuild(output, source)) {
        std::string function_exports = " -sEXPORTED_RUNTIME_METHODS=ccall -sEXPORTED_FUNCTIONS=";

        for (size_t i = 0; i < exported_functions.size(); ++i) {
            auto &func = exported_functions[i];
            function_exports += func;
            if (i != exported_functions.size() - 1)
                function_exports += ",";
        }

        std::string command = std::string("em++ ") + source + std::string(" -o ") + output + function_exports;

        std::system(command.c_str());

        std::cout << "command: " << command << std::endl;
        std::cout << "built!" << std::endl;
    }

    if (argc > 1 && std::string(argv[1]) == "-p") {
        std::system("git add .");
        std::cout << "git add ." << std::endl;
        std::system("git commit -m \"another push\"");
        std::cout << "git commit -m \"another push\"" << std::endl;
        std::system("git push");
        std::cout << "git push" << std::endl;
    }

    std::cout << "executing command: python3 -m http.server 8000" << std::endl;
    std::system("python3 -m http.server 8000");

    return 0;
}
