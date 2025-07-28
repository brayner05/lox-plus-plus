#include <iostream>
#include <fstream>
#include <string>
#include "lox.hpp"
#include "scanner/Scanner.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"

static auto interpreter = Interpreter();

static constexpr void print_value(auto&& v) {
    using Type = std::decay_t<decltype(v)>;
    
    if constexpr (std::is_empty_v<Type>)
        std::cout << "nil" << '\n';
    else if constexpr (std::is_same_v<Type, bool>)
        std::cout << (v ? "true" : "false") << '\n';
    else
        std::cout << v << '\n';
}

static void run(const std::string& source) {
    auto scanner = Scanner(source);
    auto parser = Parser(scanner.tokenize());
    auto ast = parser.parse();
    interpreter.interpret(ast);
}

static void run_repl() {
    std::string line;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        if (line == "exit") {
            break;
        }
        run(line);
    }
}

static void run_file(const std::string& path) {
    std::ifstream input_file(path);
    std::string line;

    while (std::getline(input_file, line)) {
        run(line);
    }

    if (lox::had_error())
        std::exit(65);

    if (lox::had_runtime_error())
        std::exit(70);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        run_repl();
    } else {
        run_file(argv[1]);
    }
}