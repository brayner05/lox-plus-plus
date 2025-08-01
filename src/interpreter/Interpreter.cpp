#include <iostream>
#include <variant>
#include <functional>
#include <cmath>
#include "Interpreter.hpp"
#include "../lox.hpp"

using namespace interpreter;
using namespace parser;

using scanner::Token;
using scanner::TokenType;

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

LoxValue Interpreter::visit(const Unary& unary) {
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

LoxValue Interpreter::visit(const Binary& binary) {
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

LoxValue Interpreter::visit(const Ternary& ternary) {
    auto condition = evaluate(*ternary.m_condition);
    if (is_truthy(condition))
        return evaluate(*ternary.m_success);
    else
        return evaluate(*ternary.m_failure);
}

void Interpreter::visit(const ExprStmt& stmt) {
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
        else if constexpr (std::is_same_v<T, float>)
            return std::floor(v) == v ? std::to_string(int(v)) : std::to_string(v);
        else
            return std::to_string(v);
    }, value);
}

void Interpreter::visit(const PrintStmt& stmt) {
    auto value = evaluate(*stmt.m_expr);
    std::cout << stringify(value) << '\n';
}

void Interpreter::visit(const VariableDecl& decl) {
    LoxValue initializer { std::monostate{} };
    
    if (decl.m_initializer.has_value())
        initializer = evaluate(*decl.m_initializer.value());

    m_environment->define(decl.m_name.lexeme(), initializer);
}

LoxValue Interpreter::visit(const Assign& assign) {
    auto value = evaluate(*assign.m_value);
    m_environment->assign(assign.m_name, value);
    return value;
}

LoxValue Interpreter::visit(const Grouping& grouping) {
    return evaluate(*grouping.m_inner_expr);
}

void Interpreter::visit(const Block& block) {
    auto previous = m_environment;
    try {
        m_environment = std::make_shared<Environment>(previous);
        for (const auto& stmt : block.m_statements)
            execute(*stmt);
        m_environment = previous;
    } catch (const lox::RuntimeError& error) {
        m_environment = previous;
        throw error;
    }
}

void Interpreter::visit(const IfStmt& stmt) {
    auto condition_result = evaluate(*stmt.m_condition);
    if (is_truthy(condition_result))
        execute(*stmt.m_then_clause);
    else if (stmt.m_else_clause.has_value())
        execute(*stmt.m_else_clause.value());
}

parser::LoxValue Interpreter::visit(const Logical& logical) {
    auto left = evaluate(*logical.m_left);
    auto op_type = logical.m_operator.type();

    if (op_type == TokenType::Or && is_truthy(left))
        return left;

    if (op_type == TokenType::And && !is_truthy(left))
        return left;

    return evaluate(*logical.m_right);
}

void Interpreter::visit(const WhileLoop& loop) {
    while (is_truthy(evaluate(*loop.m_condition)))
        execute(*loop.m_body);
}

void Interpreter::visit(const ForLoop& loop) {
    bool has_initializer = loop.m_initializer.has_value();
    bool has_condition = loop.m_condition.has_value();
    bool has_update = loop.m_update.has_value();

    if (has_initializer)
        execute(*loop.m_initializer.value());

    while (has_condition && is_truthy(evaluate(*loop.m_condition.value()))) {
        execute(*loop.m_body);
        if (has_update)
            evaluate(*loop.m_update.value());
    }
}