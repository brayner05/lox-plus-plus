#include "interpreter.hpp"

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

LoxValue Interpreter::visit_unary(const Unary& unary) {
    auto operation = unary.m_operator.type();
    auto argument = evaluate(*unary.m_argument);

    switch (operation) {
        case TokenType::Minus: { 
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
        case TokenType::Plus: return attempt_addition(left, right);

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
            throw std::runtime_error("Unknown binary operator.");
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