# Lox++
Lox++ is my C++ implementation of Robert Nystrom's Lox programming language from his book *Crafting Interpreters*. I highly recommend this book to anyone who is interested in making their own programming language.

## Building from Source
To build this project from source, clone this repository and run the following commands:
```sh
cd lox-plus-plus
cmake . -Bbuild
cmake --build build
```

## Running the Project
To run Lox++, ensure you have built the project, and then run the following command from the root directory:
```sh
./bin/loxpp [file.lox]
```
or to use the REPL, run:
```sh
loxpp
```

## Changes from the Original
My implementation of Lox contains some features not present in the implementation from the book. Some of these features are from challenges at the end of chapters, and some are just features I thought it would be fun to add. These features are listed below:
### Ternary Operator
#### Grammar
`Ternary → Expr '?' Expr ':' Expr`
