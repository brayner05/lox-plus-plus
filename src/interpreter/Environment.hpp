#ifndef LOX_ENVIRONMENT_HPP
#define LOX_ENVIRONMENT_HPP

#include <map>
#include <string>
#include "../parser/statements.hpp"

namespace interpreter {
    class Environment {
    private:
        std::map<std::string, parser::LoxValue> m_values {};
        std::shared_ptr<Environment> m_enclosing;

    public:
        Environment() : m_enclosing { nullptr } {}
        Environment(std::shared_ptr<Environment> enclosing) : m_enclosing(enclosing) {}

        void define(const std::string& name, const parser::LoxValue& value) {
            m_values[name] = value;
        }

        void assign(const scanner::Token& name, parser::LoxValue value) {
            if (m_values.count(name.lexeme()) != 0) {
                m_values[name.lexeme()] = value;
                return;
            }

            if (m_enclosing != nullptr) {
                m_enclosing->assign(name, value);
                return;
            }

            throw lox::RuntimeError(name, "Variable does not exist.");
        }

        const parser::LoxValue& get(const scanner::Token& name) {
            if (m_values.count(name.lexeme()) != 0) return m_values[name.lexeme()];
            if (m_enclosing != nullptr) return m_enclosing->get(name);
            throw lox::RuntimeError(name, "Variable not defined.");
        }
    };
}

#endif