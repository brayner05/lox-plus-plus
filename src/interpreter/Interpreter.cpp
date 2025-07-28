#include <iostream>
#include <variant>
#include "Interpreter.hpp"
#include "../lox.hpp"

using namespace interpreter;

using scanner::Token;
using scanner::TokenType;
using parser::LoxValue;
using parser::Expr;
using parser::Literal;
using parser::Unary;
using parser::Binary;
using parser::Ternary;
using parser::Assign;
using parser::ExprStmt;
using parser::PrintStmt;
using parser::Statement;
using parser::VariableDecl;
using parser::Grouping;

bool Interpreter::is_truthy(const LoxValue& value) {
    return std::visit([](auto&& v) -> bool {
        using ValueType = std::decay_t<decltype(v)>;

        if constexpr (std::is_empty_v<ValueType>) 
            return false;

        if constexpr (std::is_same_v<ValueType, bool>) 
            return static_cast<bool>(v);

        return true;
    }, value);
}

static void check_number_operand(const Token& operation, const LoxValue& operand) {
    std::visit([&operation](auto&& arg) {
        using ArgType = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<ArgType, float>) {
            return;
        } else {
            throw lox::RuntimeError(operation, "Expected a number.");
        }
    }, operand);
}

LoxValue Interpreter::visit_unary(const Unary& unary) {
    auto operation = unary.m_operator.type();
    auto argument = evaluate(*unary.m_argument);

    switch (operation) {
        case TokenType::Minus: { 
            check_number_operand(unary.m_operator, argument);
            auto number = std::get<float>(argument);
            return -number;
        }

        case TokenType::Bang: { 
            return !is_truthy(argument);
        }

        default: {
            throw std::runtime_error(std::string("Unknown unary operator: ") + unary.m_operator.lexeme());
        }
    }
}

static void check_number_operands(const Token& operation, const LoxValue& left, const LoxValue& right) {
    std::visit([&operation](auto&& lv, auto&& rv) {
        using LType = std::decay_t<decltype(lv)>;
        using RType = std::decay_t<decltype(rv)>;
        if constexpr (std::is_same_v<LType, float> && std::is_same_v<RType, float>) {
            return;
        } else {
            throw lox::RuntimeError(operation, std::string("Can't add ") + typeid(LType).name() + " to " + typeid(RType).name() + ".");
        }
    }, left, right);
}

LoxValue Interpreter::attempt_addition(const LoxValue& left, const LoxValue& right) {
    auto visitor = [](auto&& lv, auto&& rv) -> LoxValue {
        using LType = std::decay_t<decltype(lv)>;
        using RType = std::decay_t<decltype(rv)>;

        if constexpr (!std::is_same_v<LType, RType>)
            throw std::runtime_error(std::string("Can't add ") + typeid(LType).name() + " to " + typeid(RType).name() + ".");

        if constexpr (std::is_same_v<LType, RType> && std::is_same_v<LType, float>)
            return static_cast<float>(lv) + static_cast<float>(rv);

        if constexpr (std::is_same_v<LType, RType> && std::is_same_v<LType, std::string>)
            return static_cast<std::string>(lv) + static_cast<std::string>(rv);

        throw std::runtime_error(std::string("Can't add ") + typeid(LType).name() + " to " + typeid(RType).name() + ".");
    };

    return std::visit(visitor, left, right);
}

bool Interpreter::is_equal(const LoxValue& left, const LoxValue& right) {
    return std::visit([](auto&& lv, auto&& rv) -> bool {
        using LType = std::decay_t<decltype(lv)>;
        using RType = std::decay_t<decltype(rv)>;

        if constexpr (std::is_empty_v<LType> && std::is_empty_v<RType>)
            return true;
        else if constexpr (std::is_empty_v<LType>)
            return false;
        else if constexpr (std::is_same_v<LType, RType>)
            return lv == rv;
        else
            return false;
        
    }, left, right);
}

LoxValue Interpreter::visit_binary(const Binary& binary) {
    auto operation = binary.m_operator.type();
    auto left = evaluate(*binary.m_left);
    auto right = evaluate(*binary.m_right);

    switch (operation) {
        case TokenType::Plus: {
            check_number_operands(binary.m_operator, left, right);
            return attempt_addition(left, right);
        }

        case TokenType::Minus: {
            auto left_number = std::get<float>(left);
            auto right_number = std::get<float>(right);
            return left_number - right_number;
        }

        case TokenType::Star: {
            auto left_number = std::get<float>(left);
            auto right_number = std::get<float>(right);
            return left_number * right_number;
        }

        case TokenType::Slash: {
            auto left_number = std::get<float>(left);
            auto right_number = std::get<float>(right);
            if (right_number == 0) throw std::runtime_error("Division by 0.");
            return left_number / right_number;
        }

        case TokenType::Less: {
            auto left_number = std::get<float>(left);
            auto right_number = std::get<float>(right);
            return left_number < right_number;
        }

        case TokenType::LessEqual: {
            auto left_number = std::get<float>(left);
            auto right_number = std::get<float>(right);
            return left_number <= right_number;
        }

        case TokenType::Greater: {
            auto left_number = std::get<float>(left);
            auto right_number = std::get<float>(right);
            return left_number > right_number;
        }

        case TokenType::GreaterEqual: {
            auto left_number = std::get<float>(left);
            auto right_number = std::get<float>(right);
            return left_number >= right_number;
        }

        case TokenType::EqualEqual: return is_equal(left, right);
        case TokenType::BangEqual: return !is_equal(left, right);

        default: {
            throw lox::RuntimeError(binary.m_operator, "Unknown binary operator.");
        }
    }
}

LoxValue Interpreter::visit_ternary(const Ternary& ternary) {
    auto condition = evaluate(*ternary.m_condition);
    if (is_truthy(condition))
        return evaluate(*ternary.m_success);
    else
        return evaluate(*ternary.m_failure);
}

void Interpreter::visit_expr_stmt(const ExprStmt& stmt) {
    auto value = evaluate(*stmt.m_expr);
}

static std::string stringify(const LoxValue& value) {
    return std::visit([](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_empty_v<T>)
            return std::string("nil");
        else if constexpr (std::is_same_v<T, std::string>)
            return v;
        else if constexpr (std::is_same_v<T, bool>)
            return (v ? "true" : "false");
        else
            return std::to_string(v);
    }, value);
}

void Interpreter::visit_print_stmt(const PrintStmt& stmt) {
    auto value = evaluate(*stmt.m_expr);
    std::cout << stringify(value) << '\n';
}

void Interpreter::visit_var_decl(const VariableDecl& decl) {
    LoxValue initializer { std::monostate{} };
    
    if (decl.m_initializer.has_value())
        initializer = evaluate(*decl.m_initializer.value());

    m_environment.define(decl.m_name.lexeme(), initializer);
}

LoxValue Interpreter::visit_assign(const Assign& assign) {
    auto value = evaluate(*assign.m_value);
    m_environment.assign(assign.m_name, value);
    return value;
}

LoxValue Interpreter::visit_grouping(const Grouping& grouping) {
    return evaluate(*grouping.m_inner_expr);
}