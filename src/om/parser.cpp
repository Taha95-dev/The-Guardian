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
        case TokenType::STRUCT: return "struct";
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
           type == TokenType::STRUCT ||
           type == TokenType::IDENTIFIER ||
           type == TokenType::NUMBER ||
           type == TokenType::STRING ||
           type == TokenType::TRUE ||
           type == TokenType::FALSE ||
           type == TokenType::LPAREN;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (isAtEnd()) return nullptr;
    
    while (match(TokenType::SEMICOLON)) {
        // Skip stray semicolons
    }
    
    // Check for dict: prefix (dictionary declaration)
    if (match(TokenType::DICT_PREFIX)) {
        // Get the variable name
        Token name = expect(TokenType::IDENTIFIER, "Expected variable name after dict:");
        expect(TokenType::ASSIGN, "Expected '=' after dict:name");
        
        // Parse dictionary literal
        auto dict_literal = parseDictLiteral();
        if (!dict_literal) {
            return nullptr;
        }
        
        // Create variable definition
        auto var_def = std::make_unique<VariableDefNode>();
        var_def->name = name.value;
        var_def->value = std::move(dict_literal);
        var_def->is_const = false;
        var_def->line = name.line;
        var_def->column = name.column;
        
        match(TokenType::SEMICOLON);
        return var_def;
    }
    
    // Check for struct: prefix (struct instance)
    if (match(TokenType::STRUCT_PREFIX)) {
        // Get the struct name
        Token name = expect(TokenType::IDENTIFIER, "Expected struct name after struct:");
        // Parse struct instance
        auto instance = parseStructInstance(name.value);
        match(TokenType::SEMICOLON);
        return instance;
    }

    if (match(TokenType::FN)) {
       return parseFunctionDef();
    }
    if (match(TokenType::LET)) {
        // Check if it's an array declaration: let name: type[size];
        if (peek().type == TokenType::IDENTIFIER) {
            size_t save_pos = pos;
            Token name = peek();
            advance();
            
            if (peek().type == TokenType::COLON) {
                // It's an array declaration
                return parseArrayDecl(name);
            }
            
            // Not an array, rewind
            pos = save_pos;
        }
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
    if (match(TokenType::STRUCT)) {
        return parseStructDef();
    }
    if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    if (match(TokenType::LBRACE)) {
        return parseBlock();
    }
    
    // Check for array assignment: arr[0] = 42;
    if (peek().type == TokenType::IDENTIFIER) {
        size_t save_pos = pos;
        Token name = peek();
        advance();
        
        if (peek().type == TokenType::LBRACKET) {
            // It's an array access
            auto access = parseArrayAccess(name.value, name.line, name.column);
            if (access && match(TokenType::ASSIGN)) {
                auto value = parseExpression();
                match(TokenType::SEMICOLON);
                
                // Create an assignment node
                auto assign = std::make_unique<ArrayAssignNode>();
                assign->access = std::move(access);
                assign->value = std::move(value);
                return assign;
            }
            // If no assignment, rewind
            pos = save_pos;
        } else {
            // Not an array access, rewind
            pos = save_pos;
        }
    }
    
    // Handle standalone expressions (like function calls)
    auto expr = parseExpression();
    if (expr) {
        match(TokenType::SEMICOLON);
        return expr;
    }
    
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseStructDef() {
    auto node = std::make_unique<StructDefNode>();
    
    Token name = expect(TokenType::IDENTIFIER, "Expected struct name");
    if (name.type == TokenType::END_OF_FILE) return nullptr;
    node->name = name.value;
    node->line = name.line;
    node->column = name.column;
    
    expect(TokenType::LBRACE, "Expected '{' after struct name");
    
    while (!isAtEnd() && peek().type != TokenType::RBRACE) {
        // Parse field name (must be identifier)
        Token field_name = peek();
        if (field_name.type != TokenType::IDENTIFIER) {
            error_count++;
            std::cerr << "Error: Expected field name at line " << field_name.line << "\n";
            std::cerr << "  Got: " << field_name.value << "\n";
            advance();
            continue;
        }
        advance(); // consume field name
        
        // Expect colon
        if (!match(TokenType::COLON)) {
            error_count++;
            std::cerr << "Error: Expected ':' after field name at line " << peek().line << "\n";
            continue;
        }
        
        // Parse field type - ACCEPT TYPE KEYWORDS!
        Token field_type = peek();
        if (field_type.type == TokenType::INT ||
            field_type.type == TokenType::FLOAT ||
            field_type.type == TokenType::BOOL ||
            field_type.type == TokenType::CHAR ||
            field_type.type == TokenType::STRING ||
            field_type.type == TokenType::ANY) {
            advance(); // consume the type keyword
            node->fields.push_back({field_name.value, field_type.value});
        } else if (field_type.type == TokenType::IDENTIFIER) {
            // Custom type (like another struct name)
            advance();
            node->fields.push_back({field_name.value, field_type.value});
        } else {
            error_count++;
            std::cerr << "Error: Expected field type at line " << field_type.line << "\n";
            std::cerr << "  Got: " << field_type.value << "\n";
            advance(); // skip the problematic token
        }
        
        // Optional semicolon
        match(TokenType::SEMICOLON);
    }
    
    expect(TokenType::RBRACE, "Expected '}' after struct fields");
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseArrayLiteral() {
    auto node = std::make_unique<ArrayLiteralNode>();
    node->line = peek().line;
    node->column = peek().column;
    
    advance(); // consume [
    
    if (!match(TokenType::RBRACKET)) {
        do {
            auto element = parseExpression();
            if (element) {
                node->elements.push_back(std::move(element));
            }
        } while (match(TokenType::COMMA));
        expect(TokenType::RBRACKET, "Expected ']' after array elements");
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseArrayAccess(const std::string& name, int line, int column) {
    auto node = std::make_unique<ArrayAccessNode>();
    node->name = name;
    node->line = line;
    node->column = column;
    
    expect(TokenType::LBRACKET, "Expected '[' after array name");
    node->index = parseExpression();
    expect(TokenType::RBRACKET, "Expected ']' after index");
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseArrayDecl(const Token& name) {
    auto node = std::make_unique<ArrayDeclNode>();
    node->name = name.value;
    node->line = name.line;
    node->column = name.column;
    
    expect(TokenType::COLON, "Expected ':' after array name");
    
    // Parse type
    Token type_token = peek();
    if (type_token.type == TokenType::INT ||
        type_token.type == TokenType::FLOAT ||
        type_token.type == TokenType::BOOL ||
        type_token.type == TokenType::CHAR ||
        type_token.type == TokenType::STRING ||
        type_token.type == TokenType::ANY) {
        advance();
        node->element_type = type_token.value;
    } else {
        error_count++;
        std::cerr << "Error: Expected type keyword at line " << type_token.line << "\n";
        return nullptr;
    }
    
    expect(TokenType::LBRACKET, "Expected '[' after type");
    node->size = parseExpression();
    expect(TokenType::RBRACKET, "Expected ']' after size");
    
    // Optional semicolon
    match(TokenType::SEMICOLON);
    
    return node;
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
    
    // Skip if it's a dict: or struct: prefix (handled in parseStatement)
    if (name.value.find("dict:") == 0 || name.value.find("struct:") == 0) {
        error_count++;
        std::cerr << "Error: dict: and struct: prefixes should be used without 'let'\n";
        return nullptr;
    }
    
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
    
    // Literals
    if (token.type == TokenType::NUMBER) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::NUMBER;
        literal->value = token.value;
        return literal;
    }
    
    if (token.type == TokenType::LBRACKET) {
        return parseTypedArrayLiteral();
    }
    
    if (token.type == TokenType::DICT_PREFIX) {
        std::string name = token.value;
        // Remove "dict:" prefix
        name = name.substr(5);
        // It's a dictionary literal
        return parseDictLiteral();
    } 

    if (token.type == TokenType::STRING) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::STRING;
        literal->value = token.value;
        return literal;
    }
    
    if (token.type == TokenType::CHAR) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::CHAR;
        literal->value = "char:" + token.value;
        return literal;
    }
    
    if (token.type == TokenType::TRUE || token.type == TokenType::FALSE) {
        advance();
        auto literal = std::make_unique<LiteralNode>();
        literal->literal_type = LiteralNode::BOOLEAN;
        literal->value = token.value;
        return literal;
    }
    
    // Identifiers and their variations
    if (token.type == TokenType::IDENTIFIER) {
        std::string name = token.value;
        int line = token.line;
        int column = token.column;
        advance();
       
        // 0. Dictionary access: name:{"key"}
        if (!isAtEnd() && peek().type == TokenType::COLON) {
            // Check if it's followed by LBRACE
            size_t save_pos = pos;
            advance(); // consume :
            if (!isAtEnd() && peek().type == TokenType::LBRACE) {
                // It's a dictionary access: name:{"key"}
                auto access = std::make_unique<DictAccessNode>();
                access->name = name;
                access->line = line;
                access->column = column;
                
                advance(); // consume {
                access->key = parseExpression();
                expect(TokenType::RBRACE, "Expected '}' after dictionary key");
                return access;
            } else {
                // It's a field access: person:name
                pos = save_pos;
                return parseFieldAccess(name);
            }
        }

        // 1. Function call: name(...)
        if (!isAtEnd() && peek().type == TokenType::LPAREN) {
            auto call = std::make_unique<CallNode>();
            call->name = name;
            call->line = line;
            call->column = column;
            
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
        
        if (token.type == TokenType::STRUCT_PREFIX) {
            std::string name = token.value;
            // Remove "struct:" prefix
            name = name.substr(7);
            return parseStructInstance(name);
        }
        
        // 3. Field access: person:name
        if (!isAtEnd() && peek().type == TokenType::COLON) {
            return parseFieldAccess(name);
        }
        
        // 4. Array access: arr[0] or arr[0]:field
        if (!isAtEnd() && peek().type == TokenType::LBRACKET) {
            auto access = std::make_unique<ArrayAccessNode>();
            access->name = name;
            access->line = line;
            access->column = column;
            
            advance(); // consume [
            access->index = parseExpression();
            expect(TokenType::RBRACKET, "Expected ']' after index");
            
            // Check for field access after array access: arr[0]:field
            if (!isAtEnd() && peek().type == TokenType::COLON) {
                auto field_access = std::make_unique<FieldAccessNode>();
                field_access->struct_name = name;
                field_access->line = line;
                field_access->column = column;
                field_access->array_access = std::move(access);
                field_access->is_array_field_access = true;
                
                advance(); // consume :
                Token field = expect(TokenType::IDENTIFIER, "Expected field name");
                field_access->field_name = field.value;
                return field_access;
            }
            
            return access;
        }
        
        // 5. Regular identifier
        auto ident = std::make_unique<IdentifierNode>();
        ident->name = name;
        ident->line = line;
        ident->column = column;
        return ident;
    }
    
    // Parenthesized expression
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

std::unique_ptr<ASTNode> Parser::parseDictLiteral() {
    auto node = std::make_unique<DictLiteralNode>();
    node->line = peek().line;
    node->column = peek().column;
    
    advance(); // consume {
    
    if (!match(TokenType::RBRACE)) {
        do {
            // Parse key (can be identifier or string)
            Token key;
            if (peek().type == TokenType::STRING) {
                key = advance();
            } else if (peek().type == TokenType::IDENTIFIER) {
                key = advance();
            } else {
                error_count++;
                std::cerr << "Error: Expected key at line " << peek().line << "\n";
                return nullptr;
            }
            
            if (!match(TokenType::COLON)) {
                error_count++;
                std::cerr << "Error: Expected ':' after key at line " << peek().line << "\n";
                return nullptr;
            }
            
            auto value = parseExpression();
            node->pairs[key.value] = std::move(value);
        } while (match(TokenType::COMMA));
        expect(TokenType::RBRACE, "Expected '}' after dictionary");
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseTypedArrayLiteral() {
    auto node = std::make_unique<TypedArrayLiteralNode>();
    node->line = peek().line;
    node->column = peek().column;
    
    advance(); // consume [
    
    // Parse type
    Token type_token = peek();
    if (type_token.type == TokenType::INT ||
        type_token.type == TokenType::FLOAT ||
        type_token.type == TokenType::BOOL ||
        type_token.type == TokenType::CHAR ||
        type_token.type == TokenType::STRING ||
        type_token.type == TokenType::ANY) {
        advance();
        node->element_type = type_token.value;
    } else {
        error_count++;
        std::cerr << "Error: Expected type keyword at line " << type_token.line << "\n";
        return nullptr;
    }
    
    expect(TokenType::COLON, "Expected ':' after type");
    
    // Parse elements
    if (!match(TokenType::RBRACKET)) {
        do {
            auto element = parseExpression();
            if (element) {
                node->elements.push_back(std::move(element));
            }
        } while (match(TokenType::COMMA));
        expect(TokenType::RBRACKET, "Expected ']' after array elements");
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseStructInstance(const std::string& name) {
    auto node = std::make_unique<StructInstanceNode>();
    node->struct_name = name;
    node->line = peek().line;
    node->column = peek().column;
    
    expect(TokenType::LBRACE, "Expected '{' after struct name");
    
    while (!isAtEnd() && peek().type != TokenType::RBRACE) {
        Token field_name = expect(TokenType::IDENTIFIER, "Expected field name");
        expect(TokenType::COLON, "Expected ':' after field name");
        auto value = parseExpression();
        match(TokenType::SEMICOLON);
        
        node->fields[field_name.value] = std::move(value);
    }
    
    expect(TokenType::RBRACE, "Expected '}' after struct fields");
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseFieldAccess(const std::string& name) {
    auto node = std::make_unique<FieldAccessNode>();
    node->struct_name = name;
    node->line = peek().line;
    node->column = peek().column;
    
    expect(TokenType::COLON, "Expected ':' after struct name");
    Token field = expect(TokenType::IDENTIFIER, "Expected field name");
    node->field_name = field.value;
    
    return node;
}

} // namespace om
