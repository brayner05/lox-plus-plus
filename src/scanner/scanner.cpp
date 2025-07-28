#include <string>
#include <memory>
#include <vector>
#include <map>
#include "scanner.hpp"
#include "../lox.hpp"

std::map<std::string, TokenType> Scanner::keywords {
    { "and", TokenType::And },
    { "class", TokenType::Class },
    { "else", TokenType::Else },
    { "false", TokenType::False },
    { "fun", TokenType::Fun },
    { "for", TokenType::For },
    { "if", TokenType::If },
    { "nil", TokenType::Nil },
    { "or", TokenType::Or },
    { "print", TokenType::Print },
    { "return", TokenType::Return },
    { "super", TokenType::Super },
    { "this", TokenType::This },
    { "true", TokenType::True },
    { "var", TokenType::Var },
    { "while", TokenType::While }
};

std::vector<std::unique_ptr<Token>> Scanner::tokenize() {
    while (!is_at_end()) {
        scan_token();
        m_start = m_current;
    }
    add_token(TokenType::Eof);
    return std::move(m_tokens);
}

void Scanner::add_token(TokenType type) {
    auto lexeme = m_source.substr(m_start, m_current - m_start);
    auto token = std::make_unique<Token>(type, m_line, lexeme);
    m_tokens.push_back(std::move(token));
}

inline static bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

inline static bool is_uppercase(char ch) {
    return ch >= 'A' && ch <= 'Z';
}

inline static bool is_lowercase(char ch) {
    return ch >= 'a' && ch <= 'z';
}

inline static bool is_alphabetic(char ch) {
    return is_uppercase(ch) || is_lowercase(ch);
}

void Scanner::scan_token() {
    char ch = advance();
    switch (ch) {
        case ' ':
        case '\t':
        case '\r':
            break;

        case '\n': ++m_line; break;

        case '(': add_token(TokenType::LeftParen); break;
        case ')': add_token(TokenType::RightParen); break;
        case '{': add_token(TokenType::LeftBrace); break;
        case '}': add_token(TokenType::RightBrace); break;
        case ',': add_token(TokenType::Comma); break;
        case ';': add_token(TokenType::Semicolon); break;
        case '.': add_token(TokenType::Dot); break;
        case '?': add_token(TokenType::QuestionMark); break;
        case ':': add_token(TokenType::Colon); break;

        case '+': add_token(TokenType::Plus); break;
        case '-': add_token(TokenType::Minus); break;
        case '*': add_token(TokenType::Star); break;
        case '/': {
            if (peek() == '/') {
                while (!is_at_end() && peek() != '\n') advance();
                break;
            }
            add_token(TokenType::Slash);
            break;
        }

        case '=': add_token(match('=') ? TokenType::EqualEqual : TokenType::Equal); break;
        case '!': add_token(match('=') ? TokenType::BangEqual : TokenType::Bang); break;
        case '<': add_token(match('=') ? TokenType::LessEqual : TokenType::Less); break;
        case '>': add_token(match('=') ? TokenType::GreaterEqual : TokenType::Greater); break;

        case '\"': string(); break;

        default: {
            if (is_digit(ch)) {
                number();
                break;
            }

            if (is_alphabetic(ch)) {
                identifier();
                break;
            }

            lox::error(m_line, "Unexpected token: " + std::string { ch });
            break;
        }
    }
}

void Scanner::number() {
    while (!is_at_end() && is_digit(peek())) advance();
    if (!is_at_end() && peek() == '.') {
        advance();
        while (!is_at_end() && is_digit(peek())) advance();
    }
    auto lexeme = m_source.substr(m_start, m_current - m_start);
    m_tokens.push_back(std::make_unique<Token>(TokenType::Number, m_line, lexeme));
}

void Scanner::string() {
    while (!is_at_end() && peek() != '\"') {
        if (peek() == '\n') ++m_line;
        advance();
    }

    if (is_at_end()) {
        lox::error(m_line, "Unterminated string");
        return;
    }
    advance();
    auto lexeme = m_source.substr(m_start, m_current - m_start);
    m_tokens.push_back(std::make_unique<Token>(TokenType::String, m_line, lexeme));
}

void Scanner::identifier() {
    while (!is_at_end() && (is_alphabetic(peek()) || is_digit(peek()))) 
        advance();
    
    auto lexeme = m_source.substr(m_start, m_current - m_start);
    if (keywords.count(lexeme) != 0) {
        auto keyword = keywords[lexeme];
        add_token(keyword);
        return;
    }

    add_token(TokenType::Identifier);
}