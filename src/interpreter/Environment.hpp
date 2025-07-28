#ifndef LOX_ENVIRONMENT_HPP
#define LOX_ENVIRONMENT_HPP

#include <map>
#include <string>
#include "../parser/statements.hpp"

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
    

#endif