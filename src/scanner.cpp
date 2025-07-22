#include <string>
#include <memory>
#include <vector>
#include <map>
#include "scanner.hpp"
#include "lox.hpp"

std::map<std::string, TokenType> Scanner::keywords {
    { "class", TokenType::Class },
    { "else", TokenType::Else },
    { "false", TokenType::False },
    { "fun", TokenType::Fun },
    { "for", TokenType::For },
    { "if", TokenType::If },
    { "nil", TokenType::Nil },
    { "print", TokenType::Print },
    { "return", TokenType::Return },
    { "super", TokenType::Super },
    { "this", TokenType::This },
    { "true", TokenType::True },
    { "while", TokenType::While }
};

std::vector<std::unique_ptr<Token>> Scanner::tokenize() {
    while (!is_at_end()) {
        scan_token();
        m_start = m_current;
    }
    add_token(TokenType::Eof);
    return m_tokens;
}

void Scanner::add_token(TokenType type) {
    auto lexeme = m_source.substr(m_start, m_current - m_start);
    auto token = std::make_unique<Token>(type, m_line, lexeme);
    m_tokens.push_back(std::move(token));
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

        default: {
            lox::error(m_line, "Unexpected token: " + ch);
            break;
        }
    }
}