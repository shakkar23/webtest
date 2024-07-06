// inspired from Tsoding's SHAB header file that can be found at https://github.com/tsoding/musializer/blob/master/src/SHAB.h
#pragma once
#ifdef __cplusplus

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#ifndef SHAB_CPPSTD
#define SHAB_CPPSTD 23
#endif

#define STR2(x) #x
#define STR(x) STR2(x)
#define SHAB_CPPSTD_STR "-std=c++" STR(SHAB_CPPSTD)

#ifndef NOB_REBUILD_URSELF
#if _WIN32
#if defined(__GNUC__)
#define CXX_COMPILER "g++"
#define SHAB_REBUILD_URSELF(binary_path, source_path) CXX_COMPILER " " SHAB_CPPSTD_STR " -o " + binary_path + " " + source_path
#elif defined(__clang__)
#define CXX_COMPILER "clang++"
#define SHAB_REBUILD_URSELF(binary_path, source_path) CXX_COMPILER " " SHAB_CPPSTD_STR " -o " + binary_path + " " + source_path
#elif defined(_MSC_VER)
#define CXX_COMPILER "cl.exe"
#define SHAB_REBUILD_URSELF(binary_path, source_path) CXX_COMPILER source_path
#endif
#else
#define CXX_COMPILER "g++"
#define SHAB_REBUILD_URSELF(binary_path, source_path) CXX_COMPILER " " SHAB_CPPSTD_STR " -o " + binary_path + " " + source_path
#endif
#endif

void rebuild_yourself(int argc, char **argv, std::string file_name);

bool shab_needs_rebuild(std::string_view binary_path, std::string_view source_path);
bool shab_needs_rebuild(std::string_view binary_path, const std::vector<std::string> &source_paths);

#ifdef SHAB_IMPLEMENTATION

// Go Rebuild Urself™ Technology
//
//   How to use it:
//     int main(int argc, char** argv) {
//         GO_REBUILD_URSELF(argc, argv);
//         // actual work
//         return 0;
//     }
//
//   After your added this macro every time you run ./nobuild it will detect
//   that you modified its original source code and will try to rebuild itself
//   before doing any actual work. So you only need to bootstrap your build system
//   once.
//
//   The modification is detected by comparing the last modified times of the executable
//   and its source code. The same way the make utility usually does it.
//
//   The rebuilding is done by using the REBUILD_URSELF macro which you can redefine
//   if you need a special way of bootstraping your build system. (which I personally
//   do not recommend since the whole idea of nobuild is to keep the process of bootstrapping
//   as simple as possible and doing all of the actual work inside of the nobuild)
//
// this is a modified version of Tsoding's nob header file that was already a modified version of
// https://github.com/zhiayang/nabs 's self_update
void rebuild_yourself(int argc, char **argv, std::string file_name) {
    assert(argc > 0);
    std::string binary_path = argv[0];

    bool rebuild_is_needed = shab_needs_rebuild(binary_path, file_name);

    if (rebuild_is_needed) {
        std::string command = SHAB_REBUILD_URSELF(binary_path, file_name);

        int result = std::system(command.c_str());
        std::cout << "command:" << command << std::endl;

        if (result != 0) {
            std::cerr << "Failed to rebuild itself" << std::endl;
            std::exit(1);
        }

        for (int i = 1; i < argc; ++i) {
            binary_path += " ";
            binary_path += argv[i];
        }

        std::system(binary_path.c_str());
        std::exit(0);
    }
}

bool shab_needs_rebuild(std::string_view binary_path, std::string_view source_path) {
    return std::filesystem::last_write_time(binary_path) < std::filesystem::last_write_time(source_path);
}

bool shab_needs_rebuild(std::string_view binary_path, const std::vector<std::string> &source_paths) {
    for (auto &source : source_paths) {
        if (shab_needs_rebuild(binary_path, source))
            return true;
    }

    return false;
}

#endif  // SHAB_IMPLEMENTATION

#endif  // __cplusplus