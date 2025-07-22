#include <iostream>
#include <string>
#include "lox.hpp"

namespace lox {
    static bool had_error_ = false;

    bool had_error() {
        return had_error_;
    }

    static void report(int line, const std::string& where, const std::string& message) {
        std::cerr << "On line " << line << " at " << where << ": " << message << '\n';
    }

    void error(int line, const std::string& message) {
        report(line, "", message);
        had_error_ = true;
    }
}