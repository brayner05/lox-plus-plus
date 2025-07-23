#ifndef LOX_HPP
#define LOX_HPP

#include <string>
#include "scanner.hpp"

namespace lox {
    void error(int line, const std::string& message);
    void error(const Token& token, const std::string& message);
    bool had_error();
}

#endif