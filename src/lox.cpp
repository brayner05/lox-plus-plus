#include <iostream>
#include <string>
#include "scanner.hpp"
#include "lox.hpp"

namespace lox {
    static bool had_error_ = false;
    static bool had_runtime_error_ = false;

    bool had_error() {
        return had_error_;
    }

    bool had_runtime_error() {
        return had_runtime_error_;
    }

    static void report(int line, const std::string& where, const std::string& message) {
        std::cerr << "On line " << line << " at " << where << ": " << message << '\n';
    }

    void error(int line, const std::string& message) {
        report(line, "", message);
        had_error_ = true;
    }

    void error(const Token& token, const std::string& message) {
        if (token.type() == TokenType::Eof) {
            report(token.line(), " at end", message);
        } else {
            report(token.line(), " at '" + token.lexeme() + "'", message);
        }
    }

    void runtime_error(const RuntimeError& error) {
        std::cerr << error.what() << "\n";
    }
}