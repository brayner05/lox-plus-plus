#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Token.hpp"
#include "../util_types.hpp"

namespace scanner {
    /// @brief Transforms raw source code in text form into a series of tokens.
    class Scanner {
    private:
        const std::string& m_source;
        std::vector<std::unique_ptr<Token>> m_tokens;
        u64 m_line { 0 };
        u64 m_current { 0 };
        u64 m_start { 0 };
        static std::map<std::string, TokenType> keywords;

    public:
        Scanner(const std::string& source) : m_source(source) {}
        std::vector<std::unique_ptr<Token>> tokenize();

    private:
        bool is_at_end() const {
            return m_current >= m_source.length();
        }

        /// @brief Get the next character in the input and move ahead by 1 character.
        /// @return The next character in the input.
        char advance() {
            return m_source[m_current++];
        }

        /// @brief Get the next character in the input without moving ahead, or `'\0'` 
        /// if there are no more characters to read.
        /// @return The next character in the input.
        char peek() {
            if (is_at_end()) return '\0';
            return m_source[m_current];
        }

        /// @brief Skips the next character in the input stream if it is equal to `ch`.
        /// @param ch The character to match.
        /// @return Whether the next character is equal to `ch`.
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
}
#endif