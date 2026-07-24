#include "parser.hpp"
#include <iostream>

namespace om {

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), pos(0), error_count(0), max_errors(10) {}

bool Parser::isAtEnd() const {
    return pos >= tokens.size() || tokens[pos].type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    if (isAtEnd()) return Token(TokenType::END_OF_FILE, "", 0, 0);
    return tokens[pos];
}

Token Parser::advance() {
    if (isAtEnd()) return Token(TokenType::END_OF_FILE, "", 0, 0);
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

Token Parser::expect(TokenType type, const std::string& error) {
    if (isAtEnd()) {
        error_count++;
        std::cerr << "Error: Unexpected end of file\n";
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    }
    
    if (tokens[pos].type != type) {
        error_count++;
        std::cerr << "Error: " << error << " at line " << tokens[pos].line << "\n";
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    }
    
    return advance();
}

std::string Parser::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::FN: return "fn";
        case TokenType::LET: return "let";
        case TokenType::IDENTIFIER: return "identifier";
        case TokenType::NUMBER: return "number";
        case TokenType::STRING: return "string";
        default: return "unknown";
    }
}

bool Parser::isValidStatementStart() {
    if (isAtEnd()) return false;
    TokenType type = tokens[pos].type;
    return type == TokenType::FN ||
           type == TokenType::LET ||
           type == TokenType::IDENTIFIER;
}

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    
    while (!isAtEnd() && error_count < max_errors) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        } else {
            error_count++;
            if (!isAtEnd()) {
                advance();
            }
        }
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (isAtEnd()) return nullptr;
    
    if (match(TokenType::FN)) {
        return parseFunctionDef();
    }
    if (match(TokenType::LET)) {
        return parseVariableDef(false);
    }
    
    return nullptr;
}

std::unique_ptr<FunctionDefNode> Parser::parseFunctionDef() {
    auto node = std::make_unique<FunctionDefNode>();
    
    Token name = expect(TokenType::IDENTIFIER, "Expected function name");
    node->name = name.value;
    
    expect(TokenType::LPAREN, "Expected '(' after function name");
    expect(TokenType::RPAREN, "Expected ')' after parameters");
    
    auto body = parseBlock();
    if (body) {
        node->body = std::move(body);
    }
    
    return node;
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    auto block = std::make_unique<BlockNode>();
    
    if (!match(TokenType::LBRACE)) {
        error_count++;
        return nullptr;
    }
    
    while (!isAtEnd() && !match(TokenType::RBRACE)) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
    }
    
    return block;
}

std::unique_ptr<VariableDefNode> Parser::parseVariableDef(bool is_const) {
    auto node = std::make_unique<VariableDefNode>();
    node->is_const = is_const;
    
    Token name = expect(TokenType::IDENTIFIER, "Expected variable name");
    node->name = name.value;
    
    expect(TokenType::ASSIGN, "Expected '=' after variable name");
    node->value = parseExpression();
    match(TokenType::SEMICOLON);
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    Token token = peek();
    
    if (token.type == TokenType::NUMBER) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::NUMBER;
        literal->value = token.value;
        return literal;
    }
    
    if (token.type == TokenType::STRING) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::STRING;
        literal->value = token.value;
        return literal;
    }
    
    if (token.type == TokenType::IDENTIFIER) {
        advance();
        auto ident = std::make_unique<IdentifierNode>();
        ident->name = token.value;
        return ident;
    }
    
    return nullptr;
}

} // namespace om
