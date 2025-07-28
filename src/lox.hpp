#ifndef LOX_HPP
#define LOX_HPP

#include <string>
#include <stdexcept>
#include "scanner/Token.hpp"

namespace lox {
    void error(int line, const std::string& message);
    void error(const scanner::Token& token, const std::string& message);
    bool had_error();
    bool had_runtime_error();

    class RuntimeError : public std::runtime_error {
    private:
        const scanner::Token m_token;

    public:
        RuntimeError(const scanner::Token& token, const std::string& message)
            : std::runtime_error(message), m_token(token) {}
    };

    void runtime_error(const RuntimeError& error);
}

#endif