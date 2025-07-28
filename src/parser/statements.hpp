#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <variant>
#include <optional>
#include <memory>
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

        Expr(const Literal& literal) : m_node(literal) {}
        Expr(const Variable& identifier) : m_node(identifier) {}
        Expr(Unary&& unary) : m_node(std::move(unary)) {}
        Expr(Binary&& binary) : m_node(std::move(binary)) {}
        Expr(Ternary&& ternary) : m_node(std::move(ternary)) {}
        Expr(Assign&& assign) : m_node(std::move(assign)) {}
        Expr(Grouping&& grouping) : m_node(std::move(grouping)) {}

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
        scanner::Token m_name;
        std::optional<std::unique_ptr<Expr>> m_initializer;
        VariableDecl(const scanner::Token& name, std::optional<std::unique_ptr<Expr>> initializer)
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
                else if constexpr (std::is_same_v<T, Grouping>) return visit_grouping(node);
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
        virtual R visit_assign(const Assign& assign) = 0;
        virtual R visit_grouping(const Grouping& grouping) = 0;
    };
}
#endif