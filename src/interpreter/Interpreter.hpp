#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <variant>
#include <map>
#include <string>
#include <vector>
#include "../parser/statements.hpp"
#include "Environment.hpp"

namespace interpreter {
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

        void visit_expr_stmt(const parser::ExprStmt& stmt) override;
        void visit_print_stmt(const parser::PrintStmt& stmt) override;
        void visit_var_decl(const parser::VariableDecl& decl);
        void visit_block_stmt(const parser::Block& block) override;

        parser::LoxValue evaluate(const parser::Expr& expr) {
            return this->visit(expr);
        }

        void execute(const parser::Statement& stmt) {
            std::visit([this](auto&& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, parser::PrintStmt>)
                    visit_print_stmt(v);
                else if constexpr (std::is_same_v<T, parser::VariableDecl>)
                    visit_var_decl(v);
                else if constexpr (std::is_same_v<T, parser::Block>)
                    visit_block_stmt(v);
                else
                    visit_expr_stmt(v);
            }, stmt.m_stmt);
        }

        parser::LoxValue visit_literal(const parser::Literal& literal) override {
            return literal.m_value;
        }

        parser::LoxValue visit_identifier(const parser::Variable& identifier) override {
            return m_environment->get(identifier.m_name);
        }

        parser::LoxValue visit_unary(const parser::Unary& unary) override;
        parser::LoxValue visit_binary(const parser::Binary& binary) override;
        parser::LoxValue visit_ternary(const parser::Ternary& ternary) override;
        parser::LoxValue visit_assign(const parser::Assign& assign) override;
        parser::LoxValue visit_grouping(const parser::Grouping& grouping) override;
    };
}
#endif