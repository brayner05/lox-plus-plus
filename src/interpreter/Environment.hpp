#ifndef LOX_ENVIRONMENT_HPP
#define LOX_ENVIRONMENT_HPP

#include <map>
#include <string>
#include "../parser/statements.hpp"

namespace interpreter {
    class Environment {
    private:
        std::map<std::string, parser::LoxValue> m_values {};

    public:
        void define(const std::string& name, const parser::LoxValue& value) {
            m_values[name] = value;
        }

        void assign(const scanner::Token& name, parser::LoxValue value) {
            if (m_values.count(name.lexeme()) != 0) {
                m_values[name.lexeme()] = value;
                return;
            }
            throw lox::RuntimeError(name, "Variable does not exist.");
        }

        const parser::LoxValue& get(const scanner::Token& name) {
            if (m_values.count(name.lexeme()) != 0) return m_values[name.lexeme()];
            throw lox::RuntimeError(name, "Variable not defined.");
        }
    };
}

#endif