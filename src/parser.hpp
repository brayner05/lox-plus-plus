#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <variant>
#include <optional>
#include "lox.hpp"
#include "scanner.hpp"

struct Expr;

using LoxValue = std::variant<std::monostate, float, bool, std::string>;

template <typename T>
using maybe_ptr = std::optional<std::unique_ptr<T>>;

struct Literal {
    LoxValue m_value;
    Literal(std::monostate value) : m_value(value) {}
    Literal(float value) : m_value(value) {}
    Literal(bool value) : m_value(value) {}
    Literal(const std::string& value) : m_value(value) {}
    Literal(std::string&& value) : m_value(std::move(value)) {}
};

struct Variable {
    Token m_name;
    Variable(const Token& name) : m_name(name) {}
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

struct Assign {
    Token m_name;
    std::unique_ptr<Expr> m_initializer;
    Assign(const Token& name, std::unique_ptr<Expr> initializer)
        : m_name(name), m_initializer(std::move(initializer)) {}
};

struct Expr {
    std::variant<Literal, Variable, Unary, Binary, Ternary, Assign> m_node;

    Expr(const Literal& literal) : m_node(literal) {}
    Expr(const Variable& identifier) : m_node(identifier) {}
    Expr(Unary&& unary) : m_node(std::move(unary)) {}
    Expr(Binary&& binary) : m_node(std::move(binary)) {}
    Expr(Ternary&& ternary) : m_node(std::move(ternary)) {}
    Expr(Assign&& assign) : m_node(std::move(assign)) {}

    template <typename T>
    class Visitor;
};

struct ExprStmt {
    std::unique_ptr<Expr> m_expr;
    ExprStmt(std::unique_ptr<Expr> expr) : m_expr(std::move(expr)) {}
};

struct PrintStmt {
    std::unique_ptr<Expr> m_expr;
    PrintStmt(std::unique_ptr<Expr> expr) : m_expr(std::move(expr)) {}
};

struct VariableDecl {
    Token m_name;
    std::optional<std::unique_ptr<Expr>> m_initializer;
    VariableDecl(const Token& name, std::optional<std::unique_ptr<Expr>> initializer)
        : m_name(name), m_initializer(std::move(initializer)) {}
};

struct Statement {
    template <typename T>
    class Visitor;

    std::variant<ExprStmt, PrintStmt, VariableDecl> m_stmt;
    Statement(ExprStmt&& stmt) : m_stmt(std::move(stmt)) {}
    Statement(PrintStmt&& stmt) : m_stmt(std::move(stmt)) {}
    Statement(VariableDecl&& dec) : m_stmt(std::move(dec)) {}
};

template <typename T>
class Statement::Visitor {
public:
    virtual ~Visitor() = default;
    virtual T visit_expr_stmt(const ExprStmt& stmt) = 0;
    virtual T visit_print_stmt(const PrintStmt& stmt) = 0;
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

    std::vector<std::unique_ptr<Statement>> parse() {
        try {
            return program();
        } catch (const ParseError& parse_error) {
            return {};
        }
    }

private:
    bool is_at_end() const {
        return peek().type() == TokenType::Eof;
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

    std::vector<std::unique_ptr<Statement>> program();
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<Statement> variable_decl();
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> expr_statement();
    std::unique_ptr<Statement> print_statement();
    std::unique_ptr<Expr> expr();
    std::unique_ptr<Expr> assignment();
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

template <typename R>
class Expr::Visitor {
public:
    R visit(const Expr& expr) {
        return std::visit([this](auto&& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, Literal>) return visit_literal(node);
            else if constexpr (std::is_same_v<T, Variable>) return visit_identifier(node);
            else if constexpr (std::is_same_v<T, Unary>) return visit_unary(node);
            else if constexpr (std::is_same_v<T, Binary>) return visit_binary(node);
            else if constexpr (std::is_same_v<T, Ternary>) return visit_ternary(node);
            else if constexpr (std::is_same_v<T, Assign>) return visit_assign(node);
            else 
                throw std::runtime_error("");
        }, expr.m_node);
    }

private:
    virtual R visit_literal(const Literal& literal) = 0;
    virtual R visit_identifier(const Variable& identifier) = 0;
    virtual R visit_unary(const Unary& expr) = 0;
    virtual R visit_binary(const Binary& expr) = 0;
    virtual R visit_ternary(const Ternary& expr) = 0;
    virtual R visit_assign(const Assign& expr) = 0;
};

#endif