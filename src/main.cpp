#include <iostream>
#include <fstream>
#include <string>
#include "lox.hpp"
#include "scanner/Scanner.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"

using scanner::Scanner;
using parser::Parser;
using interpreter::Interpreter;

static auto lox_interpreter = Interpreter();

static void run(const std::string& source) {
    auto scanner = Scanner(source);
    auto parser = Parser(scanner.tokenize());
    auto ast = parser.parse();
    lox_interpreter.interpret(ast);
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