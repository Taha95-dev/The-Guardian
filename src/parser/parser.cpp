#include "parser.hpp"
#include <cctype>
#include <iostream>

namespace guardian::parser {

// ============================================
// LEXER IMPLEMENTATION
// ============================================
Lexer::Lexer(const std::string& source) 
    : source(source), pos(0), line(1), column(1) {}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    char c = peek();
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    pos++;
    return c;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek())) {
        advance();
    }
}

bool Lexer::isAtEnd() const {
    return pos >= source.length();
}

// ============================================
// PARSER IMPLEMENTATION
// ============================================
Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), pos(0), error_count(0), max_errors(10) {}

Token Parser::peek() const {
    if (isAtEnd()) return Token();
    return tokens[pos];
}

Token Parser::advance() {
    if (isAtEnd()) return Token();
    return tokens[pos++];
}

bool Parser::match(Token::Type type) {
    if (isAtEnd()) return false;
    if (tokens[pos].type == type) {
        pos++;
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const {
    return pos >= tokens.size() || tokens[pos].type == Token::Type::END_OF_FILE;
}

void Parser::error(const std::string& msg) {
    error_count++;
    std::cerr << "Error: " << msg << " at line " << peek().line << "\n";
}

// ============================================
// EXTENDED PARSER METHODS
// ============================================

std::unique_ptr<ASTNode> Parser::parseIf() {
    auto node = std::make_unique<IfNode>();
    
    // Parse condition
    if (!match(Token::Type::LPAREN)) {
        error("Expected '(' after 'if'");
        return nullptr;
    }
    node->condition = parseExpression();
    if (!match(Token::Type::RPAREN)) {
        error("Expected ')' after condition");
        return nullptr;
    }
    
    // Parse then branch
    node->then_branch = parseBlock();
    
    // Parse else branch (optional)
    if (match(Token::Type::ELSE)) {
        node->else_branch = parseBlock();
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseFor() {
    auto node = std::make_unique<ForNode>();
    
    if (!match(Token::Type::LPAREN)) {
        error("Expected '(' after 'for'");
        return nullptr;
    }
    
    // Parse initialization
    node->init = parseStatement();
    
    // Parse condition
    node->condition = parseExpression();
    if (!match(Token::Type::SEMICOLON)) {
        error("Expected ';' after condition");
        return nullptr;
    }
    
    // Parse increment
    node->increment = parseExpression();
    if (!match(Token::Type::RPAREN)) {
        error("Expected ')' after for loop header");
        return nullptr;
    }
    
    // Parse body
    node->body = parseBlock();
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseWhile() {
    auto node = std::make_unique<WhileNode>();
    
    if (!match(Token::Type::LPAREN)) {
        error("Expected '(' after 'while'");
        return nullptr;
    }
    node->condition = parseExpression();
    if (!match(Token::Type::RPAREN)) {
        error("Expected ')' after condition");
        return nullptr;
    }
    
    node->body = parseBlock();
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseReturn() {
    auto node = std::make_unique<ReturnNode>();
    
    if (!match(Token::Type::SEMICOLON)) {
        node->value = parseExpression();
        if (!match(Token::Type::SEMICOLON)) {
            error("Expected ';' after return value");
            return nullptr;
        }
    }
    
    return node;
}

} // namespace guardian::parser
