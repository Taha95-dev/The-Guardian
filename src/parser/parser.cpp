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
    
    if (!match(Token::Type::LPAREN)) {
        error("Expected '(' after 'if'");
        return nullptr;
    }
    node->condition = parseExpression();
    if (!match(Token::Type::RPAREN)) {
        error("Expected ')' after condition");
        return nullptr;
    }
    
    node->then_branch = parseBlock();
    
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
    
    node->init = parseStatement();
    node->condition = parseExpression();
    if (!match(Token::Type::SEMICOLON)) {
        error("Expected ';' after condition");
        return nullptr;
    }
    node->increment = parseExpression();
    if (!match(Token::Type::RPAREN)) {
        error("Expected ')' after for loop header");
        return nullptr;
    }
    
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

// ============================================
// THE THREE VIRTUAL FUNCTIONS — IMPLEMENTED
// ============================================

// ── Precedence Table ──
static int getPrecedence(Token::Type type) {
    switch (type) {
        case Token::Type::OR: return 1;
        case Token::Type::AND: return 2;
        case Token::Type::EQUAL:
        case Token::Type::NOT_EQUAL: return 3;
        case Token::Type::LESS:
        case Token::Type::GREATER:
        case Token::Type::LESS_EQUAL:
        case Token::Type::GREATER_EQUAL: return 4;
        case Token::Type::PLUS:
        case Token::Type::MINUS: return 5;
        case Token::Type::STAR:
        case Token::Type::SLASH:
        case Token::Type::MOD: return 6;
        default: return 0;
    }
}

// ── parseExpression ──
std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseBinary(0);
}

// ── parseBinary ──
std::unique_ptr<ASTNode> Parser::parseBinary(int min_precedence) {
    auto left = parsePrimary();
    if (!left) return nullptr;

    while (true) {
        Token op = peek();
        int prec = getPrecedence(op.type);
        if (prec < min_precedence) break;

        advance();

        auto right = parseBinary(prec + 1);
        if (!right) break;

        auto bin_op = std::make_unique<BinaryOpNode>();
        bin_op->left = std::move(left);
        bin_op->right = std::move(right);
        bin_op->op = op.value;
        left = std::move(bin_op);
    }

    return left;
}

// ── parsePrimary ──
std::unique_ptr<ASTNode> Parser::parsePrimary() {
    Token tok = peek();

    // Literals
    if (tok.type == Token::Type::NUMBER) {
        advance();
        auto lit = std::make_unique<LiteralNode>();
        lit->value = tok.value;
        return lit;
    }
    if (tok.type == Token::Type::STRING) {
        advance();
        auto lit = std::make_unique<LiteralNode>();
        lit->value = tok.value;
        return lit;
    }
    if (tok.type == Token::Type::TRUE) {
        advance();
        auto lit = std::make_unique<LiteralNode>();
        lit->value = "true";
        return lit;
    }
    if (tok.type == Token::Type::FALSE) {
        advance();
        auto lit = std::make_unique<LiteralNode>();
        lit->value = "false";
        return lit;
    }

    // Parenthesized expression
    if (tok.type == Token::Type::LPAREN) {
        advance();
        auto expr = parseExpression();
        if (!match(Token::Type::RPAREN)) {
            error("Expected ')'");
            return nullptr;
        }
        return expr;
    }

    // Variable or function call
    if (tok.type == Token::Type::IDENTIFIER) {
        advance();
        if (peek().type == Token::Type::LPAREN) {
            auto call = std::make_unique<CallNode>();
            call->name = tok.value;
            match(Token::Type::LPAREN);
            if (!match(Token::Type::RPAREN)) {
                call->args.push_back(parseExpression());
                while (match(Token::Type::COMMA)) {
                    call->args.push_back(parseExpression());
                }
                if (!match(Token::Type::RPAREN)) {
                    error("Expected ')' after arguments");
                    return nullptr;
                }
            }
            return call;
        }
        auto var = std::make_unique<VariableNode>();
        var->name = tok.value;
        return var;
    }

    error("Unexpected token in expression: " + tok.value);
    return nullptr;
}

// ── parseStatement ──
std::unique_ptr<ASTNode> Parser::parseStatement() {
    Token tok = peek();

    if (tok.type == Token::Type::KEYWORD) {
        advance();
        return parseStatement();
    }

    if (tok.value == "let") {
        advance();
        Token name = peek();
        if (name.type != Token::Type::IDENTIFIER) {
            error("Expected identifier after 'let'");
            return nullptr;
        }
        advance();
        auto var = std::make_unique<VariableNode>();
        var->name = name.value;
        if (match(Token::Type::ASSIGN)) {
            var->value = parseExpression();
        }
        if (!match(Token::Type::SEMICOLON)) {
            error("Expected ';' after assignment");
            return nullptr;
        }
        return var;
    }

    if (tok.value == "print" || tok.value == "println") {
        bool newline = (tok.value == "println");
        advance();
        if (!match(Token::Type::LPAREN)) {
            error("Expected '(' after print");
            return nullptr;
        }
        auto print_node = std::make_unique<LiteralNode>();
        print_node->type = ASTNode::Type::CUSTOM;
        auto expr = parseExpression();
        if (!match(Token::Type::RPAREN)) {
            error("Expected ')' after print argument");
            return nullptr;
        }
        if (!match(Token::Type::SEMICOLON)) {
            error("Expected ';' after print");
            return nullptr;
        }
        return expr;
    }

    if (tok.type == Token::Type::IF) {
        return parseIf();
    }

    if (tok.type == Token::Type::FOR) {
        return parseFor();
    }

    if (tok.type == Token::Type::WHILE) {
        return parseWhile();
    }

    if (tok.type == Token::Type::RETURN) {
        return parseReturn();
    }

    if (tok.type == Token::Type::IDENTIFIER) {
        return parseExpression();
    }

    advance();
    error("Unknown statement: " + tok.value);
    return nullptr;
}

// ── parseBlock ──
std::unique_ptr<ASTNode> Parser::parseBlock() {
    auto block = std::make_unique<BlockNode>();

    if (!match(Token::Type::LBRACE)) {
        error("Expected '{'");
        return nullptr;
    }

    while (!isAtEnd() && peek().type != Token::Type::RBRACE) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        } else {
            advance();
        }
    }

    if (!match(Token::Type::RBRACE)) {
        error("Expected '}'");
        return nullptr;
    }

    return block;
}

} // namespace guardian::parser
