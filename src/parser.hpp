#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "lox.hpp"
#include "scanner.hpp"

struct Expr {
    virtual ~Expr() = default;
    class Binary;
    class Unary;
    template <typename T>
    class Literal;
};

class Expr::Binary : public Expr {
private:
    std::unique_ptr<Expr> m_left;
    std::unique_ptr<Token> m_operator;
    std::unique_ptr<Expr> m_right;

public:
    Binary(std::unique_ptr<Expr> left, std::unique_ptr<Token> operator_, std::unique_ptr<Expr> right)
        : m_left(std::move(left)), m_operator(std::move(operator_)), m_right(std::move(right)) {}

    const Expr& left() const {
        return *m_left;
    }

    const Token& operator_() const {
        return *m_operator;
    }

    const Expr& right() const {
        return *m_right;
    }
};

class Expr::Unary : public Expr {
private:
    std::unique_ptr<Token> m_operator;
    std::unique_ptr<Expr> m_operand;

public:
    Unary(std::unique_ptr<Token> operator_, std::unique_ptr<Expr> operand)
        : m_operator(std::move(operator_)), m_operand(std::move(operand)) {}

    const Expr& operand() const {
        return *m_operand;
    }

    const Token& operator_() const {
        return *m_operator;
    }
};

template <typename T>
class Expr::Literal : public Expr {
private:
    T m_value;

public:
    Literal(const T& value) : m_value(value) {}

    const T& value() {
        return m_value;
    }
};

class Parser {
private:
    std::vector<std::unique_ptr<Token>> m_tokens;
    int m_position { 0 };

public:
    Parser(std::vector<std::unique_ptr<Token>> tokens) : m_tokens(std::move(tokens)) {}

    std::unique_ptr<Expr> parse() {
        return equality();
    }

private:
    bool is_at_end() const {
        return std::size_t(m_position) >= m_tokens.size();
    }

    const Token& previous() const {
        return *m_tokens[m_position - 1];
    }

    const Token& peek() const {
        return *m_tokens[m_position];
    }

    const Token& advance() {
        if (!is_at_end()) m_position++;
        return previous();
    }

    bool check(TokenType type) const {
        if (is_at_end()) return false;
        return peek().type() == type;
    }

    bool match(std::initializer_list<TokenType> types) {
        for (const auto type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    Token consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        throw error(peek(), message);
    }

    std::runtime_error error(const Token& token, const std::string& message) {
        lox::error(token, message);
        return std::runtime_error("");
    }

    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
};

#endif