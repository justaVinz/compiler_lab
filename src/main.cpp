#include "lexer/Lexer.h"
#include "parser/parser_test.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc >= 3 && std::string(argv[1]) == "--tokenize") {

        std::string file = argv[2];
        std::string fullPath = "test/lexer/" + file;

        run(file, fullPath, false);
        return 0;
    }
    if (argc >= 3 && std::string(argv[1]) == "--tokenize_verbose") {

        std::string file = argv[2];
        std::string fullPath = "test/lexer/" + file;

        run(file, fullPath, true);
        return 0;
    }
    if (argc >= 2 && std::string(argv[1]) == "--parser_test") {
        parser_test();
    }

    // --- normaler Compiler-Code ---
    std::cout << "Normal compiler mode\n";

    return 0;
}