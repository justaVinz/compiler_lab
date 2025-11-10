#include <string>

#ifndef COMPILER_LAB_TOKEN_H
#define COMPILER_LAB_TOKEN_H


class Token {
private:
    std::string tokenType;
    std::string value;
    int sourceLine;
    int sourceIndex;
public:
    Token(std::string type, std::string val, int line, int index);
    friend std::ostream& operator<<(std::ostream& os, const Token& obj);

};

#endif //COMPILER_LAB_TOKEN_H