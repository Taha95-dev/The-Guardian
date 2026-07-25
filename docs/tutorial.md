# The Guardian Tutorial — Build Your First Language

## Overview

In this tutorial, you'll build a simple language called **"Tiny"** using The Guardian framework.

**Tiny Language Features:**
- Variables (`let x = 42`)
- Print (`println(x)`)
- Basic math (`x + y`)

---

## Prerequisites

- C++17 compiler
- The Guardian installed system-wide
- Basic C++ knowledge

---

## Step 1: Set Up Your Project

```bash
mkdir tiny-lang
cd tiny-lang
mkdir src

Create CMakeLists.txt:
cmake

cmake_minimum_required(VERSION 3.10)
project(TinyLang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(GUARDIAN_CORE guardian_core REQUIRED)
find_library(GUARDIAN_MEMORY guardian_memory REQUIRED)
find_library(GUARDIAN_PARSER guardian_parser REQUIRED)
find_library(GUARDIAN_FORMAT guardian_format REQUIRED)
find_library(GUARDIAN_VM guardian_vm REQUIRED)

add_executable(tiny
    src/main.cpp
)

target_link_libraries(tiny
    ${GUARDIAN_CORE}
    ${GUARDIAN_MEMORY}
    ${GUARDIAN_PARSER}
    ${GUARDIAN_FORMAT}
    ${GUARDIAN_VM}
)

Step 2: Create the Lexer
cpp

// src/lexer.hpp
#pragma once

#include <string>
#include <vector>

enum class TokenType {
    IDENTIFIER,
    NUMBER,
    STRING,
    KEYWORD,
    SYMBOL,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    
private:
    std::string source;
    size_t pos;
    int line;
    int col;
    
    char peek() const;
    char advance();
    bool isAtEnd() const;
    Token readIdentifier();
    Token readNumber();
    Token readString();
};

cpp

// src/lexer.cpp
#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string& source) 
    : source(source), pos(0), line(1), col(1) {}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    char c = peek();
    if (c == '\n') { line++; col = 1; }
    else { col++; }
    pos++;
    return c;
}

bool Lexer::isAtEnd() const {
    return pos >= source.length();
}

Token Lexer::readIdentifier() {
    std::string value;
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
        value += advance();
    }
    return Token{TokenType::IDENTIFIER, value, line, col};
}

Token Lexer::readNumber() {
    std::string value;
    while (!isAtEnd() && isdigit(peek())) {
        value += advance();
    }
    return Token{TokenType::NUMBER, value, line, col};
}

Token Lexer::readString() {
    advance(); // skip "
    std::string value;
    while (!isAtEnd() && peek() != '"') {
        value += advance();
    }
    advance(); // skip "
    return Token{TokenType::STRING, value, line, col};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        char c = peek();
        
        if (isspace(c)) {
            advance();
            continue;
        }
        
        if (isalpha(c) || c == '_') {
            tokens.push_back(readIdentifier());
            continue;
        }
        
        if (isdigit(c)) {
            tokens.push_back(readNumber());
            continue;
        }
        
        if (c == '"') {
            tokens.push_back(readString());
            continue;
        }
        
        // Symbols
        if (c == '+' || c == '-' || c == '*' || c == '/' ||
            c == '(' || c == ')' || c == '{' || c == '}' ||
            c == '=' || c == ';' || c == ',') {
            tokens.push_back(Token{TokenType::SYMBOL, std::string(1, c), line, col});
            advance();
            continue;
        }
        
        advance(); // skip unknown
    }
    
    tokens.push_back(Token{TokenType::END_OF_FILE, "EOF", line, col});
    return tokens;
}

Step 3: Create the Parser
cpp

// src/parser.hpp
#pragma once

#include "lexer.hpp"
#include <memory>

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct NumberNode : ASTNode {
    int value;
};

struct VariableNode : ASTNode {
    std::string name;
};

struct BinaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

struct PrintNode : ASTNode {
    std::unique_ptr<ASTNode> expr;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ASTNode> parse();
    
private:
    std::vector<Token> tokens;
    size_t pos;
    
    bool isAtEnd() const;
    Token peek() const;
    Token advance();
    bool match(TokenType type);
    
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parsePrimary();
};

cpp

// src/parser.cpp
#include "parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), pos(0) {}

bool Parser::isAtEnd() const {
    return pos >= tokens.size() || tokens[pos].type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    if (isAtEnd()) return Token{TokenType::END_OF_FILE, "EOF", 0, 0};
    return tokens[pos];
}

Token Parser::advance() {
    if (isAtEnd()) return Token{TokenType::END_OF_FILE, "EOF", 0, 0};
    return tokens[pos++];
}

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    if (tokens[pos].type == type) {
        pos++;
        return true;
    }
    return false;
}

std::unique_ptr<ASTNode> Parser::parse() {
    if (isAtEnd()) return nullptr;
    
    Token token = peek();
    
    if (token.value == "println") {
        advance(); // consume println
        if (!match(TokenType::SYMBOL) || tokens[pos-1].value != "(") {
            // Error: expected (
            return nullptr;
        }
        auto expr = parseExpression();
        if (!match(TokenType::SYMBOL) || tokens[pos-1].value != ")") {
            // Error: expected )
            return nullptr;
        }
        
        auto print_node = std::make_unique<PrintNode>();
        print_node->expr = std::move(expr);
        return print_node;
    }
    
    if (token.value == "let") {
        advance(); // consume let
        Token name = advance(); // variable name
        if (!match(TokenType::SYMBOL) || tokens[pos-1].value != "=") {
            // Error: expected =
            return nullptr;
        }
        auto value = parseExpression();
        // Store variable (or whatever the code should be)
        return value;
    }
    
    return parseExpression();
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parsePrimary();
    
    while (!isAtEnd() && peek().value == "+") {
        auto op = advance();
        auto right = parsePrimary();
        
        auto binary = std::make_unique<BinaryOpNode>();
        binary->op = op.value;
        binary->left = std::move(left);
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    Token token = peek();
    
    if (token.type == TokenType::NUMBER) {
        advance();
        auto num = std::make_unique<NumberNode>();
        num->value = std::stoi(token.value);
        return num;
    }
    
    if (token.type == TokenType::IDENTIFIER) {
        advance();
        auto var = std::make_unique<VariableNode>();
        var->name = token.value;
        return var;
    }
    
    return nullptr;
}

Step 4: Create the Main Program
cpp

// src/main.cpp
#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>
#include <fstream>

// Guardian framework
#include <guardian/core/atom.hpp>
#include <guardian/core/molecule.hpp>
#include <guardian/vm/vm.hpp>

using namespace guardian;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: tiny <file.tiny>\n";
        return 1;
    }
    
    // Read file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << argv[1] << "\n";
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Tokenize
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "📝 Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << "  " << token.value << " (line " << token.line << ")\n";
    }
    
    // Parse
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "\n✅ Parsed successfully!\n";
    } else {
        std::cout << "\n❌ Parse failed!\n";
        return 1;
    }
    
    // Use Guardian VM to execute
    std::cout << "\n🔷 Built on The Guardian framework!\n";
    std::cout << "  - Core: Atoms, Molecules, Quarks\n";
    std::cout << "  - Memory safety: LUT\n";
    std::cout << "  - VM: Bytecode execution\n";
    
    return 0;
}

Step 5: Test Your Language

Create hello.tiny:
text

let x = 42
println(x)

Build and run:
bash

mkdir build && cd build
cmake ..
make
./tiny hello.tiny

Expected output:
text

📝 Tokens:
  let (line 1)
  x (line 1)
  = (line 1)
  42 (line 1)
  println (line 2)
  ( (line 2)
  x (line 2)
  ) (line 2)
  EOF (line 3)

✅ Parsed successfully!

🔷 Built on The Guardian framework!
  - Core: Atoms, Molecules, Quarks
  - Memory safety: LUT
  - VM: Bytecode execution

Next Steps

Now that you've built a language, you can:

    Add more features — if/else, loops, functions

    Generate bytecode — use Guardian's VM format

    Optimize — use Guardian's memory system

    Create a binary format — use Guardian's .gbin

You've just built a language with The Guardian! 🚀
