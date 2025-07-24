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
    Literal(std::monostate value) : m_value(value) {}
    Literal(float value) : m_value(value) {}
    Literal(bool value) : m_value(value) {}
    Literal(const std::string& value) : m_value(value) {}
    Literal(std::string&& value) : m_value(std::move(value)) {}
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
    Expr(const Literal& literal) : m_node(literal) {}
    Expr(const Identifier& identifier) : m_node(identifier) {}
    Expr(Unary&& unary) : m_node(std::move(unary)) {}
    Expr(Binary&& binary) : m_node(std::move(binary)) {}
    Expr(Ternary&& ternary) : m_node(std::move(ternary)) {}
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

class AstPrinter {
private:
    std::ostream& m_stream;

public:
    AstPrinter(std::ostream& stream) : m_stream(stream) {}

    void print(const Expr& expr) {
        std::visit([this](auto&& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, Literal>) print_literal(node);
            else if constexpr (std::is_same_v<T, Identifier>) print_identifier(node);
            else if constexpr (std::is_same_v<T, Unary>) print_unary(node);
            else if constexpr (std::is_same_v<T, Binary>) print_binary(node);
            else if constexpr (std::is_same_v<T, Ternary>) print_ternary(node);
            else 
                throw std::runtime_error("");
        }, expr.m_node);
    }

private:
    void print_literal(const Literal& literal) {
        std::visit([this](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::monostate>) 
                m_stream << "nil";
            else if constexpr (std::is_same_v<T, bool>)
                m_stream << (value ? "true" : "false");
            else 
                m_stream << value;
        }, literal.m_value);
    }

    void print_identifier(const Identifier& identifier) {
        m_stream << identifier.m_name;
    }

    void print_unary(const Unary& expr) {
        m_stream << "(";
        print(*expr.m_argument);
        m_stream << " " << expr.m_operator.lexeme() << ")";
    }

    void print_binary(const Binary& expr) {
        m_stream << "(";
        print(*expr.m_left);
        m_stream << " " << expr.m_operator.lexeme() << " ";
        print(*expr.m_right);
        m_stream << ")";
    }

    void print_ternary(const Ternary& expr) {
        m_stream << "(";
        print(*expr.m_condition);
        m_stream << " ? ";
        print(*expr.m_success);
        m_stream << " : ";
        print(*expr.m_failure);
        m_stream << ")";
    }
};

#endif