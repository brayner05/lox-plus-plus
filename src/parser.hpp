#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "lox.hpp"
#include "scanner.hpp"

struct Expr {
    virtual ~Expr() = default;
    virtual void print(std::ostream& stream) const = 0;
    class Ternary;
    class Binary;
    class Unary;
    template <typename T>
    class Literal;
    class Identifier;
};

inline std::ostream& operator<<(std::ostream& stream, const Expr& expr) { 
    expr.print(stream);
    return stream; 
}

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

    void print(std::ostream& stream) const override;
};

inline void Expr::Ternary::print(std::ostream& stream) const { 
    stream << "(" << argument_1() << " " << operator_1().lexeme() << " "  << argument_2() << " " << operator_2().lexeme() << " " << argument_3() << ')';
}

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

    void print(std::ostream& stream) const override;
};

inline void Expr::Binary::print(std::ostream& stream) const { 
    stream << "(" << left() << " " << operator_().lexeme() << " "  << right() << ')';
}

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

    void print(std::ostream& stream) const override;
};

inline void Expr::Unary::print(std::ostream& stream) const {
    stream << "(" << operator_().lexeme() << " " << operand() << ')';
}

template <typename T>
class Expr::Literal : public Expr {
private:
    T m_value;

public:
    Literal(const T& value) : m_value(value) {}

    const T& value() const {
        return m_value;
    }

    void print(std::ostream& stream) const override;
};

template <typename T>
inline void Expr::Literal<T>::print(std::ostream& stream) const { 
    stream << value();
}

template <>
inline void Expr::Literal<bool>::print(std::ostream& stream) const {
    stream << (value() ? "true" : "false");
}

class Expr::Identifier : public Expr::Literal<std::string> {
    using Expr::Literal<std::string>::Literal;
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

class AstPrinter {
private:
    std::ostream& m_stream;

public:
    AstPrinter(std::ostream& stream) : m_stream(stream) {}

    void print(const Expr& root) {
        root.print(m_stream);
        std::cout << '\n';
    }
};

#endif