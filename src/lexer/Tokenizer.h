# ifndef COMPILER_LAB_TOKENIZER_H
# define COMPILER_LAB_TOKENIZER_H
# include "../helper/structs/TokenizeAttempt.h"
# include <vector>
# include <optional>
# include <utility>

class Tokenizer {
public:
    static TokenizeAttempt tokenize(const char* source, bool isVerbose);
    static std::pair<std::vector<Token>, std::optional<std::pair<int, int>>> tokenizeSeq(std::string source, bool isVerbose);
};

# endif //COMPILER_LAB_TOKENIZER_H