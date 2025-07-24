#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <variant>
#include "lox.hpp"
#include "scanner.hpp"

struct Expr;

struct Literal {
    std::variant<std::monostate, float, bool, std::string> m_value;

    template <typename T>
    Literal(T&& value) : m_value(std::forward<T>(value)) {}
};

struct Identifier {
    std::string m_name;
    Identifier(const std::string& name) : m_name(name) {}
};

struct Unary {
    Token m_operator;
    std::unique_ptr<Expr> m_argument;
    Unary(const Token& operation, std::unique_ptr<Expr> argument)
        : m_operator(operation), m_argument(std::move(argument)) {}
};

struct Binary {
    Token m_operator;
    std::unique_ptr<Expr> m_left;
    std::unique_ptr<Expr> m_right;
    Binary(const Token& operation, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : m_operator(operation), m_left(std::move(left)), m_right(std::move(right)) {}
};

struct Ternary {
    std::unique_ptr<Expr> m_condition;
    std::unique_ptr<Expr> m_success;
    std::unique_ptr<Expr> m_failure;
    Ternary(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> success, std::unique_ptr<Expr> failure)
        : m_condition(std::move(condition)), m_success(std::move(success)), m_failure(std::move(failure)) {}
};

struct Expr {
    std::variant<Literal, Identifier, Unary, Binary, Ternary> m_node;

    template <typename T>
    Expr(T&& node) : m_node(std::forward<T>(node)) {}
};

class Parser {
public:
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& message) : std::runtime_error(message) {}
    };

private:
    std::vector<std::unique_ptr<Token>> m_tokens;
    int m_position { 0 };

public:
    Parser(std::vector<std::unique_ptr<Token>> tokens) : m_tokens(std::move(tokens)) {}

    std::unique_ptr<Expr> parse() {
        try {
            return expr();
        } catch (const ParseError& parse_error) {
            return nullptr;
        }
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

    ParseError error(const Token& token, const std::string& message) {
        lox::error(token, message);
        return ParseError(message);
    }

    std::unique_ptr<Expr> expr();
    std::unique_ptr<Expr> ternary();
    std::unique_ptr<Expr> validate_equality();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> validate_compound();
    std::unique_ptr<Expr> compound();
    std::unique_ptr<Expr> validate_comparison();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> validate_term();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> validate_factor();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
    void synchronize();
};

#endif