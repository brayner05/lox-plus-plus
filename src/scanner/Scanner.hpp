#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Token.hpp"

class Scanner {
private:
    const std::string& m_source;
    std::vector<std::unique_ptr<Token>> m_tokens;
    int m_line { 0 };
    int m_current { 0 };
    int m_start { 0 };
    static std::map<std::string, TokenType> keywords;

public:
    Scanner(const std::string& source) : m_source(source) {}
    std::vector<std::unique_ptr<Token>> tokenize();

private:
    bool is_at_end() const {
        return std::size_t(m_current) >= m_source.length();
    }

    char advance() {
        return m_source[m_current++];
    }

    char peek() {
        if (is_at_end()) return '\0';
        return m_source[m_current];
    }

    bool match(char ch) {
        if (is_at_end()) return false;
        if (peek() != ch) return false;
        advance();
        return true;
    }

    void add_token(TokenType type);
    void scan_token();
    void number();
    void string();
    void identifier();
};

#endif