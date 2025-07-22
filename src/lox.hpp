#ifndef LOX_HPP
#define LOX_HPP

#include <string>

namespace lox {
    void error(int line, const std::string& message);
    bool had_error();
}

#endif