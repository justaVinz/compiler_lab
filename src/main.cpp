#include "lexer/Lexer.h"
#include <iostream>
#include "parser/ParserRunner.h"

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

    if (argc >= 3 && std::string(argv[1]) == "--parse") {
        std::string file = argv[2];
        std::string fullPath = "../../test/parser/" + file;
        bool success = runParser(file, fullPath, false);
        return success ? 0 : 1;
    }

    if (argc >= 3 && std::string(argv[1]) == "--parse_verbose") {
        std::string file = argv[2];
        std::string fullPath = "../../test/parser/" + file;
        bool success = runParser(file, fullPath, true);
        return success ? 0 : 1;
    }

    // --- normaler Compiler-Code ---
    std::cout << "Normal compiler mode\n";

    return 0;
}