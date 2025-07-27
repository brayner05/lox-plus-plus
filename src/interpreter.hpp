#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <variant>
#include <map>
#include <string>
#include "parser.hpp"

class Environment {
private:
    std::map<std::string, LoxValue> m_values {};

public:
    void define(const std::string& name, const LoxValue& value) {
        m_values[name] = value;
    }

    void assign(const Token& name, LoxValue value) {
        if (m_values.count(name.lexeme()) != 0) {
            m_values[name.lexeme()] = value;
            return;
        }
        throw lox::RuntimeError(name, "Variable does not exist.");
    }

    const LoxValue& get(const Token& name) {
        if (m_values.count(name.lexeme()) != 0) return m_values[name.lexeme()];
        throw lox::RuntimeError(name, "Variable not defined.");
    }
};

class Interpreter : Expr::Visitor<LoxValue>, Statement::Visitor<void> {
private:
    Environment m_environment {};
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
    void visit_var_decl(const VariableDecl& decl);

    LoxValue evaluate(const Expr& expr) {
        return this->visit(expr);
    }

    void execute(const Statement& stmt) {
        std::visit([this](auto&& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, PrintStmt>)
                visit_print_stmt(v);
            else if constexpr  (std::is_same_v<T, VariableDecl>)
                visit_var_decl(v);
            else
                visit_expr_stmt(v);
        }, stmt.m_stmt);
    }

    LoxValue visit_literal(const Literal& literal) override {
        return literal.m_value;
    }

    LoxValue visit_identifier(const Variable& identifier) override {
        return m_environment.get(identifier.m_name);
    }

    LoxValue visit_unary(const Unary& unary) override;
    LoxValue visit_binary(const Binary& binary) override;
    LoxValue visit_ternary(const Ternary& ternary) override;
    LoxValue visit_assign(const Assign& assign) override;
};

#endif