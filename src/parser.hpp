#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <variant>
#include "lox.hpp"
#include "scanner.hpp"

struct Expr {
    /// Type denoting the type of expression.
    enum class Type { Ternary, Binary, Unary, Literal, Identifier };

    /// Ternary expression.
    class Ternary;

    /// Binary expression.
    class Binary;

    /// Unary expression.
    class Unary;

    /// Identifier expression.
    class Identifier;

    /// Template class for literal types.
    class Literal;

    // Abstract methods.
    virtual ~Expr() = default;
    virtual Type type() const = 0;

    virtual std::string to_string() const = 0;
};

class Expr::Ternary : public Expr {
private:
    std::unique_ptr<Expr> m_arg_1;
    std::unique_ptr<Token> m_op_1;
    std::unique_ptr<Expr> m_arg_2;
    std::unique_ptr<Token> m_op_2;
    std::unique_ptr<Expr> m_arg_3;

public:
    Ternary(
        std::unique_ptr<Expr> arg_1, 
        std::unique_ptr<Token> op_1, 
        std::unique_ptr<Expr> arg_2,
        std::unique_ptr<Token> op_2,
        std::unique_ptr<Expr> arg_3
    ) : m_arg_1(std::move(arg_1)), m_op_1(std::move(op_1)), m_arg_2(std::move(arg_2)), m_op_2(std::move(op_2)), m_arg_3(std::move(arg_3)) {}

    const Expr& argument_1() const {
        return *m_arg_1;
    }

    const Expr& argument_2() const {
        return *m_arg_2;
    }

    const Expr& argument_3() const {
        return *m_arg_3;
    }

    const Token& operator_1() const {
        return *m_op_1;
    }

    const Token& operator_2() const {
        return *m_op_2;
    }

    Type type() const override {
        return Type::Ternary;
    }

    std::string to_string() const override {
        auto stream = std::stringstream();
        stream << "(" << m_arg_1->to_string() << " " << m_op_1->lexeme() << " ";
        stream << m_arg_2->to_string() << " " << m_op_2->lexeme() << " ";
        stream << m_arg_3->to_string() << ")";
        return stream.str();
    }
};

class Expr::Binary : public Expr {
private:
    std::unique_ptr<Expr> m_left;
    std::unique_ptr<Token> m_operator;
    std::unique_ptr<Expr> m_right;

public:
    Binary(std::unique_ptr<Expr> left, std::unique_ptr<Token> operator_, std::unique_ptr<Expr> right)
        : m_left(std::move(left)), m_operator(std::move(operator_)), m_right(std::move(right)) {}

    const Expr& left() const {
        return *m_left;
    }

    const Token& operator_() const {
        return *m_operator;
    }

    const Expr& right() const {
        return *m_right;
    }

    Type type() const override {
        return Type::Binary;
    }

    std::string to_string() const override {
        auto stream = std::stringstream();
        stream << "(" << m_left->to_string() << " " << m_operator->lexeme() << " " << m_right->to_string() << ")";
        return stream.str();
    }
};

class Expr::Unary : public Expr {
private:
    std::unique_ptr<Token> m_operator;
    std::unique_ptr<Expr> m_operand;

public:
    Unary(std::unique_ptr<Token> operator_, std::unique_ptr<Expr> operand)
        : m_operator(std::move(operator_)), m_operand(std::move(operand)) {}

    const Expr& operand() const {
        return *m_operand;
    }

    const Token& operator_() const {
        return *m_operator;
    }

    Type type() const override {
        return Type::Unary;
    }

    std::string to_string() const override {
        auto stream = std::stringstream();
        stream << "(" << m_operator->lexeme() << " " << m_operand->to_string() << ")";
        return stream.str();
    }
};

class Expr::Literal : public Expr {
public:
    using Value = std::variant<std::monostate, float, bool, std::string>;

private:
    Value m_value;

public:
    template<typename T>
    Literal(T&& value) : m_value(std::forward<T>(value)) {}

    virtual Type type() const override {
        return Type::Literal;
    }

    template <typename T>
    T value() const {
        return std::get<T>(m_value);
    }

    std::string to_string() const override {
        return std::visit([](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::monostate>) return "nil";
            else if constexpr (std::is_same_v<T, bool>) return val ? "true" : "false";
            else if constexpr (std::is_same_v<T, std::string>) return val;
            else return std::to_string(val);
        }, m_value);
    }
};

class Expr::Identifier : public Expr {
private:
    std::string m_name;

public:
    Identifier(const std::string& name) : m_name(name) {}

    Type type() const override {
        return Type::Identifier;
    }

    std::string to_string() const override {
        auto stream = std::stringstream();
        stream << m_name;
        return stream.str();
    }
};

class Parser {
public:
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& message) : std::runtime_error(message) {}
    };

private:
    std::vector<std::unique_ptr<Token>> m_tokens;
    int m_position { 0 };

public:
    Parser(std::vector<std::unique_ptr<Token>> tokens) : m_tokens(std::move(tokens)) {}

    std::unique_ptr<Expr> parse() {
        try {
            return expr();
        } catch (const ParseError& parse_error) {
            return nullptr;
        }
    }

private:
    bool is_at_end() const {
        return std::size_t(m_position) >= m_tokens.size();
    }

    const Token& previous() const {
        return *m_tokens[m_position - 1];
    }

    const Token& peek() const {
        return *m_tokens[m_position];
    }

    const Token& advance() {
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

    Token consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        throw error(peek(), message);
    }

    ParseError error(const Token& token, const std::string& message) {
        lox::error(token, message);
        return ParseError(message);
    }

    std::unique_ptr<Expr> expr();
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
    void synchronize();
};

#endif