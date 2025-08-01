#ifndef LOX_TOKEN_HPP
#define LOX_TOKEN_HPP

#include <ostream>
#include "../util_types.hpp"

namespace scanner {
    enum class TokenType {
        LeftParen, RightParen, LeftBrace, RightBrace,
        Comma, Dot, Minus, Plus, Semicolon, Slash, Star,
        QuestionMark, Colon,
    
        Bang, BangEqual,
        Equal, EqualEqual,
        Greater, GreaterEqual,
        Less, LessEqual,
    
        Identifier, String, Number,
    
        And, Class, Else, False, Fun, For, If, Nil, Or,
        Print, Return, Super, This, True, Var, While,
    
        Eof
    };

    inline std::ostream& operator<<(std::ostream& os, TokenType type) {
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
            case TokenType::QuestionMark: return os << "QuestionMark";
            case TokenType::Colon:        return os << "Colon";

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
            case TokenType::Var:          return os << "Var";
            case TokenType::While:        return os << "While";

            case TokenType::Eof:          return os << "Eof";

            default:                      return os << "UnknownTokenType";
        }
    }

    /// @brief Represents a single unit of code scanned directly from 
    /// the source code. That is, anything in the source code that has any
    /// meaning and/or value.
    class Token {
    private:
        TokenType m_type;
        u64 m_line;
        std::string m_lexeme;

    public:
        Token(TokenType type, u64 line, const std::string& lexeme)
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
        return stream << "[Token " << token.type() << ", \"" << token.lexeme() << "\"]";
    }

    }
#endif