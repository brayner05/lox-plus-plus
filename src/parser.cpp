#include <memory>
#include "parser.hpp"
#include "lox.hpp"

std::vector<std::unique_ptr<Statement>> Parser::program() {
    auto statements = std::vector<std::unique_ptr<Statement>>();
    while (!is_at_end())
        statements.push_back(declaration());

    return statements;
}

std::unique_ptr<Statement> Parser::statement() {
    if (match({ TokenType::Print }))
        return print_statement();
    else
        return expr_statement();
}

std::unique_ptr<Statement> Parser::expr_statement() {
    auto expression = expr();
    consume(TokenType::Semicolon, "Expected ';'.");
    return std::make_unique<Statement>(
        ExprStmt {
            std::move(expression)
        }
    );
}

std::unique_ptr<Statement> Parser::print_statement() {
    auto expression = expr();
    consume(TokenType::Semicolon, "Expected ';'.");
    return std::make_unique<Statement>(
        PrintStmt {
            std::move(expression)
        }
    );
}

std::unique_ptr<Expr> Parser::expr() {
    return ternary();
}

std::unique_ptr<Expr> Parser::ternary() {
    auto condition = validate_equality();
    if (!match({ TokenType::QuestionMark })) return condition;

    auto operator_1 = previous();

    auto success = expr();
    consume(TokenType::Colon, "Expected ':'.");

    auto operator_2 = previous();

    auto failure = expr();
    return std::make_unique<Expr>(
        Ternary {
            std::move(condition),
            std::move(success),
            std::move(failure)
        }
    );
}

std::unique_ptr<Expr> Parser::validate_equality() {
    if (match({ TokenType::EqualEqual, TokenType::BangEqual }))
        throw error(peek(), "Expected an expression");

    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    auto left = validate_compound();

    while (match({ TokenType::BangEqual, TokenType::EqualEqual })) {
        auto& operation = previous();
        auto right = validate_compound();
        left = std::make_unique<Expr>(
            Binary {
                operation,
                std::move(left), 
                std::move(right)
            }
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::validate_compound() {
    if (match({ TokenType::Comma })) throw error(peek(), "Expected an expression.");
    return compound();
}

std::unique_ptr<Expr> Parser::compound() {
    auto left = validate_comparison();

    while (match({ TokenType::Comma })) {
        auto& operation = previous();
        auto right = validate_comparison();
        left = std::make_unique<Expr>(
            Binary {
                operation,
                std::move(left), 
                std::move(right)
            }
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::validate_comparison() {
    if (match({ TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual }))
        throw error(peek(), "Expected an expression.");
    
    return comparison();
}

std::unique_ptr<Expr> Parser::comparison() {
    auto left = term();

    while (match({ TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual })) {
        auto& operation = previous();
        auto right = term();
        left = std::make_unique<Expr>(
            Binary {
                operation,
                std::move(left), 
                std::move(right)
            }
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::validate_term() {
    if (match({ TokenType::Plus, TokenType::Minus })) throw error(peek(), "Expected an expression.");
    return term();
}

std::unique_ptr<Expr> Parser::term() {
    auto left = validate_factor();

    while (match({ TokenType::Plus, TokenType::Minus })) {
        auto& operation = previous();
        auto right = validate_factor();
        left = std::make_unique<Expr>(
            Binary {
                operation,
                std::move(left), 
                std::move(right)
            }
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::validate_factor() {
    if (match({ TokenType::Star, TokenType::Slash })) throw error(peek(), "Expected an expression.");
    return factor();
}

std::unique_ptr<Expr> Parser::factor() {
    auto left = unary();

    while (match({ TokenType::Star, TokenType::Slash })) {
        auto& operation = previous();
        auto right = unary();
        left = std::make_unique<Expr>(
            Binary {
                operation,
                std::move(left), 
                std::move(right)
            }
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({ TokenType::Bang, TokenType::Minus })) {
        auto& operation = previous();
        auto argument = primary();
        return std::make_unique<Expr>(Unary { operation, std::move(argument) });
    }

    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({ TokenType::True })) 
        return std::make_unique<Expr>(Literal { true });
    
    if (match({ TokenType::False }))
        return std::make_unique<Expr>(Literal { false });

    if (match({ TokenType::Nil }))
        return std::make_unique<Expr>(Literal { std::monostate {} });

    if (match({ TokenType::Number }))
        return std::make_unique<Expr>(Literal { float(std::stod(previous().lexeme())) });

    if (match({ TokenType::String })) {
        auto& lexeme = previous().lexeme();
        return std::make_unique<Expr>(Literal { lexeme.substr(1, lexeme.length() - 2) });
    }

    if (match({ TokenType::Identifier }))
        return std::make_unique<Expr>(Variable { previous() });

    if (match({ TokenType::LeftParen })) {
        auto expression = expr();
        consume(TokenType::RightParen, "Expected ')' after expression.");
        return expression;
    }

    throw error(peek(), "Expected an expression.");
}

void Parser::synchronize() {
    advance();
    while (!is_at_end()) {
        if (previous().type() == TokenType::Semicolon) return;

        switch (peek().type()) {
            case TokenType::Class: case TokenType::For: case TokenType::Fun:
            case TokenType::If: case TokenType::Print: case TokenType::Return:
            case TokenType::Var: case TokenType::While:
                return;
                
            default: break;
        }

        advance();
    }
}

std::unique_ptr<Statement> Parser::variable_decl() {
    auto name = consume(TokenType::Identifier, "Expected an identifier.");

    std::optional<std::unique_ptr<Expr>> initializer {};
    if (match({ TokenType::Equal }))
        initializer = expr();

    consume(TokenType::Semicolon, "Expected ';'.");
    return std::make_unique<Statement>(
        VariableDecl {
            name,
            std::move(initializer)
        }
    );
}

std::unique_ptr<Statement> Parser::declaration() {
    try {
        if (match({ TokenType::Var })) return variable_decl();
        return statement();
    } catch(const ParseError& error) {
        synchronize();
        return nullptr;
    }
}