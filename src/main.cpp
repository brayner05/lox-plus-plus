#include <iostream>
#include <fstream>
#include <string>
#include "lox.hpp"
#include "scanner.hpp"
#include "parser.hpp"

static void run(const std::string& source) {
    auto scanner = Scanner(source);
    auto parser = Parser(scanner.tokenize());
    auto ast = parser.parse();
    return;
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
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        run_repl();
    } else {
        run_file(argv[1]);
    }
}