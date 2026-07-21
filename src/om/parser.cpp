#include "parser.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include <iostream>
#include <memory>

namespace om {

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), pos(0), error_count(0), max_errors(10) {}

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    int consecutive_errors = 0;
    
    while (!isAtEnd() && error_count < max_errors) {
        // Skip semicolons and unexpected tokens
        while (!isAtEnd() && !isValidStatementStart()) {
            if (peek().type == TokenType::SEMICOLON) {
                // Skip stray semicolons
                std::cout << "  (Skipping stray semicolon)\n";
                advance();
                continue;
            }
            if (peek().type != TokenType::END_OF_FILE) {
                std::cerr << "Warning: Skipping unexpected token: " << peek().value << " at line " << peek().line << "\n";
                advance();
                consecutive_errors++;
                if (consecutive_errors > 10) {
                    std::cerr << "Error: Too many consecutive errors, stopping\n";
                    return program;
                }
            } else {
                break;
            }
        }
        
        if (isAtEnd()) break;
        
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
            consecutive_errors = 0;
        } else {
            error_count++;
            consecutive_errors++;
            if (!isAtEnd()) {
                advance();
            }
            if (consecutive_errors > 10) {
                std::cerr << "Error: Too many consecutive errors, stopping\n";
                break;
            }
        }
    }
    
    if (error_count >= max_errors) {
        std::cerr << "Error: Too many errors (" << error_count << "), stopping\n";
    }
    
    return program;
}

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
        std::cerr << "  Expected: " << tokenTypeToString(type) << "\n";
        std::cerr << "  Got: " << tokens[pos].value << "\n";
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    }
    
    return advance();
}

std::string Parser::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::FN: return "fn";
        case TokenType::LET: return "let";
        case TokenType::CONST: return "const";
        case TokenType::IF: return "if";
        case TokenType::ELSE: return "else";
        case TokenType::FOR: return "for";
        case TokenType::WHILE: return "while";
        case TokenType::RETURN: return "return";
        case TokenType::TRUE: return "true";
        case TokenType::FALSE: return "false";
        case TokenType::IDENTIFIER: return "identifier";
        case TokenType::NUMBER: return "number";
        case TokenType::STRING: return "string";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::SEMICOLON: return ";";
        case TokenType::ASSIGN: return "=";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "unknown";
    }
}

bool Parser::isValidStatementStart() {
    if (isAtEnd()) return false;
    TokenType type = tokens[pos].type;
    return type == TokenType::FN ||
           type == TokenType::LET ||
           type == TokenType::CONST ||
           type == TokenType::IF ||
           type == TokenType::FOR ||
           type == TokenType::WHILE ||
           type == TokenType::RETURN ||
           type == TokenType::LBRACE ||
           type == TokenType::IDENTIFIER ||
           type == TokenType::NUMBER ||
           type == TokenType::STRING ||
           type == TokenType::TRUE ||
           type == TokenType::FALSE ||
           type == TokenType::LPAREN;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (isAtEnd()) return nullptr;
    
    // Skip stray semicolons
    while (match(TokenType::SEMICOLON)) {
        // Just skip them
    }
    
    if (match(TokenType::FN)) {
        return parseFunctionDef();
    }
    if (match(TokenType::LET)) {
        return parseVariableDef(false);
    }
    if (match(TokenType::CONST)) {
        return parseVariableDef(true);
    }
    if (match(TokenType::IF)) {
        return parseIfStatement();
    }
    if (match(TokenType::FOR)) {
        return parseForLoop();
    }
    if (match(TokenType::WHILE)) {
        return parseWhileLoop();
    }
    if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    if (match(TokenType::LBRACE)) {
        return parseBlock();
    }
    
    // Handle standalone expressions (like function calls)
    auto expr = parseExpression();
    if (expr) {
        // Consume optional semicolon
        match(TokenType::SEMICOLON);
        return expr;
    }
    
    return nullptr;
}

std::unique_ptr<FunctionDefNode> Parser::parseFunctionDef() {
    auto node = std::make_unique<FunctionDefNode>();
    
    Token name = expect(TokenType::IDENTIFIER, "Expected function name");
    if (name.type == TokenType::END_OF_FILE) return nullptr;
    
    node->name = name.value;
    node->line = name.line;
    node->column = name.column;
    
    expect(TokenType::LPAREN, "Expected '(' after function name");
    
    if (!match(TokenType::RPAREN)) {
        do {
            Token param = expect(TokenType::IDENTIFIER, "Expected parameter name");
            if (param.type != TokenType::END_OF_FILE) {
                node->params.push_back(param.value);
            }
        } while (match(TokenType::COMMA));
        expect(TokenType::RPAREN, "Expected ')' after parameters");
    }
    
    auto body = parseBlock();
    if (body) {
        node->body = std::move(body);
    }
    
    return node;
}

std::unique_ptr<VariableDefNode> Parser::parseVariableDef(bool is_const) {
    auto node = std::make_unique<VariableDefNode>();
    node->is_const = is_const;
    
    Token name = expect(TokenType::IDENTIFIER, "Expected variable name");
    if (name.type == TokenType::END_OF_FILE) return nullptr;
    
    node->name = name.value;
    node->line = name.line;
    node->column = name.column;
    
    expect(TokenType::ASSIGN, "Expected '=' after variable name");
    
    node->value = parseExpression();
    
    // Optional semicolon
    match(TokenType::SEMICOLON);
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    auto node = std::make_unique<IfStatementNode>();
    
    expect(TokenType::LPAREN, "Expected '(' after 'if'");
    node->condition = parseExpression();
    expect(TokenType::RPAREN, "Expected ')' after condition");
    
    node->then_branch = parseBlock();
    
    if (match(TokenType::ELSE)) {
        node->else_branch = parseBlock();
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseForLoop() {
    auto node = std::make_unique<ForLoopNode>();
    
    expect(TokenType::LPAREN, "Expected '(' after 'for'");
    node->init = parseStatement();
    node->condition = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after condition");
    node->increment = parseExpression();
    expect(TokenType::RPAREN, "Expected ')' after for loop header");
    
    node->body = parseBlock();
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseWhileLoop() {
    auto node = std::make_unique<WhileLoopNode>();
    
    expect(TokenType::LPAREN, "Expected '(' after 'while'");
    node->condition = parseExpression();
    expect(TokenType::RPAREN, "Expected ')' after condition");
    
    node->body = parseBlock();
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseReturnStatement() {
    auto node = std::make_unique<ReturnStatementNode>();
    
    if (!match(TokenType::SEMICOLON)) {
        node->value = parseExpression();
        match(TokenType::SEMICOLON);
    }
    
    return node;
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    auto block = std::make_unique<BlockNode>();
    
    // Expect opening brace
    if (!match(TokenType::LBRACE)) {
        error_count++;
        std::cerr << "Error: Expected '{' at line " << peek().line << "\n";
        return nullptr;
    }
    
    // Parse statements until closing brace or end of file
    while (!isAtEnd() && !match(TokenType::RBRACE)) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        } else {
            // If we can't parse a statement, skip to the next token
            if (!isAtEnd()) {
                advance();
            }
        }
    }
    
    return block;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseBinaryOp(0);
}

std::unique_ptr<ASTNode> Parser::parseBinaryOp(int min_precedence) {
    auto left = parsePrimary();
    if (!left) return nullptr;
    
    while (true) {
        Token op = peek();
        int precedence = getPrecedence(op.type);
        
        if (precedence < min_precedence || precedence == 0) {
            break;
        }
        
        advance();
        
        auto right = parseBinaryOp(precedence + 1);
        if (!right) {
            error_count++;
            std::cerr << "Error: Expected expression after operator at line " << op.line << "\n";
            return nullptr;
        }
        
        auto binary = std::make_unique<BinaryOpNode>();
        binary->op = op.value;
        binary->left = std::move(left);
        binary->right = std::move(right);
        left = std::move(binary);
    }
    
    return left;
}

int Parser::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::OR: return 1;
        case TokenType::AND: return 2;
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL: return 3;
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL: return 4;
        case TokenType::PLUS:
        case TokenType::MINUS: return 5;
        case TokenType::STAR:
        case TokenType::SLASH: return 6;
        case TokenType::MOD: return 6;
        default: return 0;
    }
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (isAtEnd()) {
        error_count++;
        std::cerr << "Error: Unexpected end of file\n";
        return nullptr;
    }
    
    Token token = peek();
    
    if (token.type == TokenType::NUMBER) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::NUMBER;
        literal->value = token.value;
        return literal;
    }
    
    if (token.type == TokenType::STRING) {
    
    if (token.type == TokenType::CHAR) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::CHAR;
        literal->value = token.value;
        return literal;
    }
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::STRING;
        literal->value = token.value;
        return literal;
    }
    
    if (token.type == TokenType::TRUE || token.type == TokenType::FALSE) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::BOOLEAN;
        literal->value = token.value;
        return literal;
    }
    
    if (token.type == TokenType::IDENTIFIER) {
        advance();
        // Check if it's a function call
        if (!isAtEnd() && peek().type == TokenType::LPAREN) {
            auto call = std::make_unique<CallNode>();
            call->name = token.value;
            call->line = token.line;
            call->column = token.column;
            
            advance(); // consume (
            if (!match(TokenType::RPAREN)) {
                do {
                    auto arg = parseExpression();
                    if (arg) {
                        call->args.push_back(std::move(arg));
                    }
                } while (match(TokenType::COMMA));
                expect(TokenType::RPAREN, "Expected ')' after arguments");
            }
            return call;
        }
        
        auto ident = std::make_unique<IdentifierNode>();
        ident->name = token.value;
        ident->line = token.line;
        ident->column = token.column;
        return ident;
    }
    
    if (token.type == TokenType::LPAREN) {
        advance();
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    
    error_count++;
    std::cerr << "Error: Unexpected token: '" << token.value << "' at line " << token.line << "\n";
    return nullptr;
}

} // namespace om
