#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <variant>
#include <map>
#include <string>
#include <vector>
#include "../parser/statements.hpp"
#include "Environment.hpp"

namespace interpreter {
    /// @brief Performs a tree walk on a given AST, executing each statement along the way.
    class Interpreter : parser::Expr::Visitor<parser::LoxValue>, parser::Statement::Visitor<void> {
    private:
        std::shared_ptr<Environment> m_environment { std::make_shared<Environment>() };
        bool is_truthy(const parser::LoxValue& value);
        bool is_equal(const parser::LoxValue& left, const parser::LoxValue& right);
        parser::LoxValue attempt_addition(const parser::LoxValue& left, const parser::LoxValue& right);

    public:
        void interpret(const std::vector<std::unique_ptr<parser::Statement>>& program) {
            try {
                for (const auto& stmt : program) {
                    execute(*stmt);
                }
            } catch (lox::RuntimeError& error) {
                lox::runtime_error(error);
            }
        }

        void execute(const parser::Statement& stmt) {
            visit_stmt(stmt);
        }

        parser::LoxValue evaluate(const parser::Expr& expr) {
            return visit_expr(expr);
        }

        void visit(const parser::ExprStmt& stmt) override;
        void visit(const parser::PrintStmt& stmt) override;
        void visit(const parser::VariableDecl& decl) override;
        void visit(const parser::Block& block) override;
        void visit(const parser::IfStmt& stmt) override;
        void visit(const parser::WhileLoop& loop) override;
        
        parser::LoxValue visit(const parser::Unary& unary) override;
        parser::LoxValue visit(const parser::Binary& binary) override;
        parser::LoxValue visit(const parser::Ternary& ternary) override;
        parser::LoxValue visit(const parser::Assign& assign) override;
        parser::LoxValue visit(const parser::Grouping& grouping) override;
        parser::LoxValue visit(const parser::Logical& logical) override;

        parser::LoxValue visit(const parser::Literal& literal) override {
            return literal.m_value;
        }

        parser::LoxValue visit(const parser::Variable& identifier) override {
            return m_environment->get(identifier.m_name);
        }
    };
}
#endif