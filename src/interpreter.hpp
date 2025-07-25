#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <variant>
#include "parser.hpp"

class Interpreter : Expr::Visitor<LoxValue> {
private:
    bool is_truthy(const LoxValue& value);
    bool is_equal(const LoxValue& left, const LoxValue& right);
    LoxValue attempt_addition(const LoxValue& left, const LoxValue& right);

public:
    LoxValue evaluate(const Expr& expr) {
        return this->visit(expr);
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