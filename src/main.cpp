#include "lexer/Lexer.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc >= 3 && std::string(argv[1]) == "--tokenize") {

        std::string file = argv[2];
        std::string fullPath = "test/lexer/" + file;

        run(file, fullPath);
        return 0;
    }

    // --- normaler Compiler-Code ---
    std::cout << "Normal compiler mode\n";

    return 0;
}