#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <variant>
#include <optional>
#include <memory>
#include <type_traits>
#include "../lox.hpp"
#include "../scanner/Token.hpp"

namespace parser {
    struct Expr;

    using LoxValue = std::variant<std::monostate, float, bool, std::string>;

    struct Literal {
        LoxValue m_value;
        Literal(std::monostate value) : m_value(value) {}
        Literal(float value) : m_value(value) {}
        Literal(bool value) : m_value(value) {}
        Literal(const std::string& value) : m_value(value) {}
        Literal(std::string&& value) : m_value(std::move(value)) {}
    };

    struct Variable {
        scanner::Token m_name;
        Variable(const scanner::Token& name) : m_name(name) {}
    };

    struct Unary {
        scanner::Token m_operator;
        std::unique_ptr<Expr> m_argument;
        Unary(const scanner::Token& operation, std::unique_ptr<Expr> argument)
            : m_operator(operation), m_argument(std::move(argument)) {}
    };

    struct Binary {
        scanner::Token m_operator;
        std::unique_ptr<Expr> m_left;
        std::unique_ptr<Expr> m_right;
        Binary(const scanner::Token& operation, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
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
        scanner::Token m_name;
        std::unique_ptr<Expr> m_value;
        Assign(const scanner::Token& name, std::unique_ptr<Expr> value)
            : m_name(name), m_value(std::move(value)) {}
    };

    struct Grouping {
        std::unique_ptr<Expr> m_inner_expr;
        Grouping(std::unique_ptr<Expr> inner_expr) : m_inner_expr(std::move(inner_expr)) {}
    };

    struct Expr {
        std::variant<Literal, Variable, Unary, Binary, Ternary, Assign, Grouping> m_node;

        template <typename T>
        Expr(T&& expr) : m_node(std::forward<T>(expr)) {}

        template <typename T>
        class Visitor;
    };

    struct Statement;

    struct ExprStmt {
        std::unique_ptr<Expr> m_expr;
        ExprStmt(std::unique_ptr<Expr> expr) : m_expr(std::move(expr)) {}
    };

    struct PrintStmt {
        std::unique_ptr<Expr> m_expr;
        PrintStmt(std::unique_ptr<Expr> expr) : m_expr(std::move(expr)) {}
    };

    struct VariableDecl {
        scanner::Token m_name;
        std::optional<std::unique_ptr<Expr>> m_initializer;
        VariableDecl(const scanner::Token& name, std::optional<std::unique_ptr<Expr>> initializer)
            : m_name(name), m_initializer(std::move(initializer)) {}
    };

    struct Block {
        std::vector<std::unique_ptr<Statement>> m_statements;
        Block(std::vector<std::unique_ptr<Statement>>&& statements)
            : m_statements(std::move(statements)) {}
    };

    struct IfStmt {
        std::unique_ptr<Expr> m_condition;
        std::unique_ptr<Statement> m_then_clause;
        std::optional<std::unique_ptr<Statement>> m_else_clause;

        IfStmt(
            std::unique_ptr<Expr> condition,
            std::unique_ptr<Statement> then_clause,
            std::optional<std::unique_ptr<Statement>> else_clause
        ) : m_condition(std::move(condition)), m_then_clause(std::move(then_clause)), m_else_clause(std::move(else_clause)) {}
    };

    struct Statement {
        std::variant<ExprStmt, PrintStmt, VariableDecl, Block, IfStmt> m_stmt;

        template <typename T>
        Statement(T&& stmt) : m_stmt(std::forward<T>(stmt)) {}

        template <typename T>
        class Visitor;
    };

    template <typename R>
    class Statement::Visitor {
    public:
        R visit_stmt(const Statement& stmt) {
            return std::visit([this](auto&& v) -> R {
                return visit(v);
            }, stmt.m_stmt);
        }

        virtual ~Visitor() = default;
        virtual R visit(const ExprStmt& stmt) = 0;
        virtual R visit(const PrintStmt& stmt) = 0;
        virtual R visit(const Block& block) = 0;
        virtual R visit(const VariableDecl& decl) = 0;
        virtual R visit(const IfStmt& stmt) = 0;
    };


    template <typename R>
    class Expr::Visitor {
    public:
        R visit_expr(const Expr& expr) {
            return std::visit([this](auto&& v) -> R {
                return visit(v);
            }, expr.m_node);
        }

        virtual R visit(const Literal& literal) = 0;
        virtual R visit(const Variable& identifier) = 0;
        virtual R visit(const Unary& expr) = 0;
        virtual R visit(const Binary& expr) = 0;
        virtual R visit(const Ternary& expr) = 0;
        virtual R visit(const Assign& assign) = 0;
        virtual R visit(const Grouping& grouping) = 0;
    };
}
#endif