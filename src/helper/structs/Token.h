#pragma once
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
    Token();

    std::string getTokenType() const { return tokenType; }
    void setTokenType(const std::string &token_type) { tokenType = token_type; }
    std::string getValue() const { return value; }
    void setValue(const std::string &val) { value = val; }
    int getSourceLine() const { return sourceLine; }
    void setSourceLine(int source_line) { sourceLine = source_line; }
    int getSourceIndex() const { return sourceIndex; }
    void setSourceIndex(int source_index) { sourceIndex = source_index; }

    friend std::ostream& operator<<(std::ostream& os, const Token& obj);

};

#endif //COMPILER_LAB_TOKEN_H