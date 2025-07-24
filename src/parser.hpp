#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
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

    class Visitor;

    // Abstract methods.
    virtual ~Expr() = default;
    virtual Type type() const = 0;
    virtual void accept(Visitor& visitor) const = 0;
};

class Expr::Visitor {
public:
    virtual void visit_ternary(const Ternary& expr) = 0;

    virtual void visit_binary(const Binary& expr) = 0;

    virtual void visit_unary(const Unary& expr) = 0;

    virtual void visit_literal(const Literal& literal) = 0;

    virtual void visit_identifier(const Identifier& identifier) = 0;

    virtual ~Visitor() = default;
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

    void accept(Visitor& visitor) const override {
        visitor.visit_ternary(*this);
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

    void accept(Visitor& visitor) const override {
        visitor.visit_binary(*this);
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

    void accept(Visitor& visitor) const override {
        visitor.visit_unary(*this);
    }
};

class Expr::Literal : public Expr {
public:
    template <typename T>
    class Typed;

    class Nil;

public:
    virtual Type type() const override {
        return Type::Literal;
    }

    virtual std::string to_lexeme() const = 0;

    void accept(Visitor& visitor) const override {
        visitor.visit_literal(*this);
    }
};

template <typename T>
class Expr::Literal::Typed : public Expr::Literal {
private:
    T m_value;

public:
    Typed(const T& value) : m_value(value) {}

    const T& value() const {
        return m_value;
    }

    std::string to_lexeme() const override {
        auto stream = std::stringstream();
        stream << value();
        return stream.str();
    }
};

template <>
inline std::string Expr::Literal::Typed<bool>::to_lexeme() const {
    return value() ? "true" : "false";
}

class Expr::Literal::Nil : public Expr::Literal {
public:
    std::string to_lexeme() const override {
        return "nil";
    }
};

class Expr::Identifier : public Expr::Literal::Typed<std::string> {
public:
    using Expr::Literal::Typed<std::string>::Typed;

    Type type() const override {
        return Type::Identifier;
    }

    void accept(Visitor& visitor) const override {
        visitor.visit_identifier(*this);
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

class AstPrinter : public Expr::Visitor {
private:
    std::ostream& m_stream;

public:
    AstPrinter(std::ostream& stream) : m_stream(stream) {}

    void print(const Expr& expr) {
        expr.accept(*this);
    }

private:
    void visit_unary(const Expr::Unary& expr) override {
        m_stream << "(" << expr.operator_().lexeme() << " ";
        print(expr.operand());
        m_stream << ')';
    }

    void visit_binary(const Expr::Binary& expr) override {
        m_stream << "(";
        print(expr.left());
        m_stream << " " << expr.operator_().lexeme() << " ";
        print(expr.right());
        m_stream << ')';
    }

    void visit_ternary(const Expr::Ternary& expr) override {
        m_stream << "(";
        print(expr.argument_1());
        m_stream << " " << expr.operator_1().lexeme() << " ";
        print(expr.argument_2());
        m_stream << " " << expr.operator_2().lexeme() << " ";
        print(expr.argument_3());
        m_stream << ')';
    }

    void visit_identifier(const Expr::Identifier& identifier) override {
        m_stream << identifier.value();
    }

    void visit_literal(const Expr::Literal& number) override {
        m_stream << number.to_lexeme();
    }
};

#endif