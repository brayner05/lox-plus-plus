#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <variant>
#include "parser.hpp"

class Interpreter : Expr::Visitor<LoxValue>, Statement::Visitor<void> {
private:
    bool is_truthy(const LoxValue& value);
    bool is_equal(const LoxValue& left, const LoxValue& right);
    LoxValue attempt_addition(const LoxValue& left, const LoxValue& right);

public:
    void interpret(const std::vector<std::unique_ptr<Statement>>& program) {
        try {
            for (const auto& stmt : program) {
                execute(*stmt);
            }
        } catch (lox::RuntimeError& error) {
            lox::runtime_error(error);
        }
    }

    void visit_expr_stmt(const ExprStmt& stmt) override;
    void visit_print_stmt(const PrintStmt& stmt) override;

    LoxValue evaluate(const Expr& expr) {
        return this->visit(expr);
    }

    void execute(const Statement& stmt) {
        std::visit([this](auto&& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, PrintStmt>)
                visit_print_stmt(v);
            else
                visit_expr_stmt(v);
        }, stmt.m_stmt);
    }

    LoxValue visit_literal(const Literal& literal) override {
        return literal.m_value;
    }

    LoxValue visit_identifier(const Identifier& identifier) override {
        throw std::runtime_error("Not Implemented.");
    }

    LoxValue visit_unary(const Unary& unary) override;
    LoxValue visit_binary(const Binary& binary) override;
    LoxValue visit_ternary(const Ternary& ternary) override;
};

#endif