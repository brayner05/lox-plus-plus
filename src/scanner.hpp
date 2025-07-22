#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

enum class TokenType {
    LeftParen, RightParen, LeftBrace, RightBrace,
    Comma, Dot, Minus, Plus, Semicolon, Slash, Star,
  
    Bang, BangEqual,
    Equal, EqualEqual,
    Greater, GreaterEqual,
    Less, LessEqual,
  
    Identifier, String, Number,
  
    And, Class, Else, False, Fun, For, If, Nil, Or,
    Print, Return, Super, This, True, While,
  
    Eof
};

std::ostream& operator<<(std::ostream& os, TokenType type) {
    switch (type) {
        case TokenType::LeftParen:    return os << "LeftParen";
        case TokenType::RightParen:   return os << "RightParen";
        case TokenType::LeftBrace:    return os << "LeftBrace";
        case TokenType::RightBrace:   return os << "RightBrace";
        case TokenType::Comma:        return os << "Comma";
        case TokenType::Dot:          return os << "Dot";
        case TokenType::Minus:        return os << "Minus";
        case TokenType::Plus:         return os << "Plus";
        case TokenType::Semicolon:    return os << "Semicolon";
        case TokenType::Slash:        return os << "Slash";
        case TokenType::Star:         return os << "Star";

        case TokenType::Bang:         return os << "Bang";
        case TokenType::BangEqual:    return os << "BangEqual";
        case TokenType::Equal:        return os << "Equal";
        case TokenType::EqualEqual:   return os << "EqualEqual";
        case TokenType::Greater:      return os << "Greater";
        case TokenType::GreaterEqual: return os << "GreaterEqual";
        case TokenType::Less:         return os << "Less";
        case TokenType::LessEqual:    return os << "LessEqual";

        case TokenType::Identifier:   return os << "Identifier";
        case TokenType::String:       return os << "String";
        case TokenType::Number:       return os << "Number";

        case TokenType::And:          return os << "And";
        case TokenType::Class:        return os << "Class";
        case TokenType::Else:         return os << "Else";
        case TokenType::False:        return os << "False";
        case TokenType::Fun:          return os << "Fun";
        case TokenType::For:          return os << "For";
        case TokenType::If:           return os << "If";
        case TokenType::Nil:          return os << "Nil";
        case TokenType::Or:           return os << "Or";
        case TokenType::Print:        return os << "Print";
        case TokenType::Return:       return os << "Return";
        case TokenType::Super:        return os << "Super";
        case TokenType::This:         return os << "This";
        case TokenType::True:         return os << "True";
        case TokenType::While:        return os << "While";

        case TokenType::Eof:          return os << "Eof";

        default:                      return os << "UnknownTokenType";
    }
}

class Token {
private:
    TokenType m_type;
    int m_line;
    std::string m_lexeme;

public:
    Token(TokenType type, int line, const std::string& lexeme)
        : m_type(type), m_line(line), m_lexeme(lexeme) {}

    TokenType type() const {
        return m_type;
    }

    int line() const {
        return m_line;
    }

    const std::string& lexeme() const {
        return m_lexeme;
    }
};

inline std::ostream& operator<<(std::ostream& stream, const Token& token) {
    stream << "[Token " << token.type() << ", \"" << token.lexeme() << "\"]";
}

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
        return m_line >= m_source.length();
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
        return peek() == ch;
    }

    void add_token(TokenType type);
    void scan_token();
    void number();
    void string();
    void identifier();
};

#endif