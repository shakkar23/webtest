#define SHAB_IMPLEMENTATION
#include "shab.hpp"

// g++ -std=c++23 -o shab shab.cpp

int main(int argc, char **argv) {
    rebuild_yourself(argc, argv, __FILE__);

    std::string output = "main.js";
    std::vector<std::string> sources = {"main.cpp", "util.cpp", "Tetris/Game.cpp", "Tetris/Move.cpp", "Tetris/Piece.cpp", "Tetris/rng.cpp", "Tetris/VSGame.cpp"};
    for (auto &source : sources) {
        source = "cpp/" + source;
    }

    std::vector<std::string> exported_functions = {"_update", "_keyup", "_keydown"};

    if (shab_needs_rebuild(output, sources)) {
        std::string function_exports = " -sEXPORTED_RUNTIME_METHODS=ccall -sEXPORTED_FUNCTIONS=";

        for (size_t i = 0; i < exported_functions.size(); ++i) {
            auto &func = exported_functions[i];
            function_exports += func;
            if (i != exported_functions.size() - 1)
                function_exports += ",";
        }

        std::string command = std::string("em++ -std=c++23 ");

        for (auto &source : sources) {
            command += source + " ";
        }

        command += std::string(" -o ") + output + function_exports;

        if (std::system(command.c_str())) {
            std::cout << "command: " << command << std::endl;
            std::cout << "could not be built!" << std::endl;
            return 1;
        }

        std::cout << "command: " << command << std::endl;
        std::cout << "built!" << std::endl;
    }

    if (argc > 1 && std::string(argv[1]) == "-p") {
        std::system("git add .");
        std::cout << "git add ." << std::endl;

        std::string message = argc > 1 ? argv[2] : "another push";
        std::string command = "git commit -m \"" + message + "\"";
        std::system(command.c_str());
        std::cout << command << std::endl;
        
        std::system("git push");
        std::cout << "git push" << std::endl;
    }

    std::cout << "executing command: python3 -m http.server 8000" << std::endl;
    std::system("python3 -m http.server 8000");

    return 0;
}
