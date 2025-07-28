#ifndef LOX_PARSER_HPP
#define LOX_PARSER_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include "../scanner/Token.hpp"
#include "statements.hpp"

namespace parser {
    class Parser {
    public:
        using TokenType = scanner::TokenType;

        class ParseError : public std::runtime_error {
        public:
            ParseError(const std::string& message) : std::runtime_error(message) {}
        };

    private:
        std::vector<std::unique_ptr<scanner::Token>> m_tokens;
        int m_position { 0 };

    public:
        Parser(std::vector<std::unique_ptr<scanner::Token>> tokens) : m_tokens(std::move(tokens)) {}

        std::vector<std::unique_ptr<Statement>> parse() {
            try {
                return program();
            } catch (const ParseError& parse_error) {
                return {};
            }
        }

    private:
        bool is_at_end() const {
            return peek().type() == TokenType::Eof;
        }

        const scanner::Token& previous() const {
            return *m_tokens[m_position - 1];
        }

        const scanner::Token& peek() const {
            return *m_tokens[m_position];
        }

        const scanner::Token& advance() {
            if (!is_at_end()) m_position++;
            return previous();
        }

        bool check(TokenType type) const {
            if (is_at_end()) return false;
            return peek().type() == type;
        }

        bool match(std::initializer_list<TokenType> types) {
            for (const auto type : types) {
                if (check(type)) {
                    advance();
                    return true;
                }
            }
            return false;
        }

        scanner::Token consume(TokenType type, const std::string& message) {
            if (check(type)) return advance();
            throw error(peek(), message);
        }

        ParseError error(const scanner::Token& token, const std::string& message) {
            lox::error(token, message);
            return ParseError(message);
        }

        std::vector<std::unique_ptr<Statement>> program();
        std::unique_ptr<Statement> declaration();
        std::unique_ptr<Statement> variable_decl();
        std::unique_ptr<Statement> statement();
        std::unique_ptr<Statement> expr_statement();
        std::unique_ptr<Statement> print_statement();
        std::unique_ptr<Expr> expr();
        std::unique_ptr<Expr> assign();
        std::unique_ptr<Expr> ternary();
        std::unique_ptr<Expr> validate_equality();
        std::unique_ptr<Expr> equality();
        std::unique_ptr<Expr> validate_compound();
        std::unique_ptr<Expr> compound();
        std::unique_ptr<Expr> validate_comparison();
        std::unique_ptr<Expr> comparison();
        std::unique_ptr<Expr> validate_term();
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> validate_factor();
        std::unique_ptr<Expr> factor();
        std::unique_ptr<Expr> unary();
        std::unique_ptr<Expr> primary();
        std::unique_ptr<Expr> grouping();
        void synchronize();
    };
}    

#endif