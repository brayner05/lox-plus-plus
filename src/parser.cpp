#include <memory>
#include "parser.hpp"
#include "lox.hpp"

std::unique_ptr<Expr> Parser::expr() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    auto left = compound_expr();

    while (match({ TokenType::BangEqual, TokenType::EqualEqual })) {
        auto& operation = previous();
        auto right = compound_expr();
        left = std::make_unique<Expr::Binary>(
            std::move(left), 
            std::make_unique<Token>(operation), 
            std::move(right)
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::compound_expr() {
    auto left = comparison();

    while (match({ TokenType::Comma })) {
        auto& operation = previous();
        auto right = comparison();
        left = std::make_unique<Expr::Binary>(
            std::move(left), 
            std::make_unique<Token>(operation), 
            std::move(right)
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto left = term();

    while (match({ TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual })) {
        auto& operation = previous();
        auto right = term();
        left = std::make_unique<Expr::Binary>(
            std::move(left), 
            std::make_unique<Token>(operation), 
            std::move(right)
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::term() {
    auto left = factor();

    while (match({ TokenType::Plus, TokenType::Minus })) {
        auto& operation = previous();
        auto right = factor();
        left = std::make_unique<Expr::Binary>(
            std::move(left), 
            std::make_unique<Token>(operation), 
            std::move(right)
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::factor() {
    auto left = unary();

    while (match({ TokenType::Star, TokenType::Slash })) {
        auto& operation = previous();
        auto right = unary();
        left = std::make_unique<Expr::Binary>(
            std::move(left), 
            std::make_unique<Token>(operation), 
            std::move(right)
        );
    }

    return left;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({ TokenType::Bang, TokenType::Minus })) {
        auto& operation = previous();
        auto argument = primary();
        return std::make_unique<Expr::Unary>(std::make_unique<Token>(operation), std::move(argument));
    }

    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({ TokenType::True })) 
        return std::make_unique<Expr::Literal<bool>>(true);
    
    if (match({ TokenType::False }))
        return std::make_unique<Expr::Literal<bool>>(false);

    if (match({ TokenType::Nil }))
        return std::make_unique<Expr::Literal<void*>>(nullptr);

    if (match({ TokenType::Number }))
        return std::make_unique<Expr::Literal<float>>(std::stod(previous().lexeme()));

    if (match({ TokenType::String })) {
        auto& lexeme = previous().lexeme();
        return std::make_unique<Expr::Literal<std::string>>(lexeme.substr(1, lexeme.length() - 2));
    }

    if (match({ TokenType::Identifier }))
        return std::make_unique<Expr::Identifier>(previous().lexeme());

    if (match({ TokenType::LeftParen })) {
        auto expression = equality();
        consume(TokenType::RightParen, "Expected ')' after expression.");
        return expression;
    }

    throw error(peek(), "Expected an expression.");
}