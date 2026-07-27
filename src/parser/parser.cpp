#include "parser.hpp"
#include <cctype>
#include <optional>

namespace guardian::parser {

// ── LEXER ──
Lexer::Lexer(const std::string& source) 
    : source(source), pos(0), line(1), column(1) {}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[pos];
}

char Lexer::peekNext() const {
    if (pos + 1 >= source.length()) return '\0';
    return source[pos + 1];
}

char Lexer::advance() {
    char c = peek();
    pos++;
    if (c == '\n') { line++; column = 1; }
    else { column++; }
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

void Lexer::error(const std::string& msg) {
    std::cerr << "Lexer error: " << msg << " at line " << line << "\n";
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        char c = peek();
        
        if (std::isspace(c)) { skipWhitespace(); continue; }
        if (c == '/' && peekNext() == '/') {
            while (!isAtEnd() && peek() != '\n') advance();
            tokens.push_back(Token(Token::Type::COMMENT, "//", line, column));
            continue;
        }
        if (c == '"') { tokens.push_back(scanString()); continue; }
        if (c == '\'') { tokens.push_back(scanChar()); continue; }
        if (std::isdigit(c)) { tokens.push_back(scanNumber()); continue; }
        if (std::isalpha(c) || c == '_') { tokens.push_back(scanIdentifier()); continue; }
        if (auto tok = scanSymbol()) { tokens.push_back(*tok); continue; }
        
        error("Unexpected character: " + std::string(1, c));
        advance();
    }
    
    tokens.push_back(Token(Token::Type::END_OF_FILE));
    return tokens;
}

Token Lexer::scanIdentifier() {
    std::string value;
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        value += advance();
    }
    
    if (value == "let") return Token(Token::Type::LET, value, line, column);
    if (value == "fn") return Token(Token::Type::FN, value, line, column);
    if (value == "if") return Token(Token::Type::IF, value, line, column);
    if (value == "else") return Token(Token::Type::ELSE, value, line, column);
    if (value == "for") return Token(Token::Type::FOR, value, line, column);
    if (value == "while") return Token(Token::Type::WHILE, value, line, column);
    if (value == "return") return Token(Token::Type::RETURN, value, line, column);
    if (value == "true") return Token(Token::Type::TRUE, value, line, column);
    if (value == "false") return Token(Token::Type::FALSE, value, line, column);
    if (value == "null") return Token(Token::Type::NULL_TOKEN, value, line, column);
    
    return Token(Token::Type::IDENTIFIER, value, line, column);
}

Token Lexer::scanNumber() {
    std::string value;
    bool isFloat = false;
    while (!isAtEnd() && (std::isdigit(peek()) || peek() == '.')) {
        if (peek() == '.') isFloat = true;
        value += advance();
    }
    return Token(Token::Type::NUMBER, value, line, column);
}

Token Lexer::scanString() {
    advance(); // consume "
    std::string value;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            advance();
            switch (peek()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case '"': value += '"'; break;
                default: value += peek(); break;
            }
            advance();
        } else {
            value += advance();
        }
    }
    if (!isAtEnd()) advance();
    return Token(Token::Type::STRING, value, line, column);
}

Token Lexer::scanChar() {
    advance(); // consume '
    std::string value;
    if (!isAtEnd() && peek() != '\'') {
        value += advance();
    }
    if (!isAtEnd() && peek() == '\'') advance();
    return Token(Token::Type::CHAR, value, line, column);
}

std::optional<Token> Lexer::scanSymbol() {
    char c = advance();
    switch (c) {
        case '+': return Token(Token::Type::PLUS, "+", line, column);
        case '-': return Token(Token::Type::MINUS, "-", line, column);
        case '*': return Token(Token::Type::STAR, "*", line, column);
        case '/': return Token(Token::Type::SLASH, "/", line, column);
        case '%': return Token(Token::Type::MOD, "%", line, column);
        case '=':
            if (peek() == '=') { advance(); return Token(Token::Type::EQUAL, "==", line, column); }
            return Token(Token::Type::ASSIGN, "=", line, column);
        case '!':
            if (peek() == '=') { advance(); return Token(Token::Type::NOT_EQUAL, "!=", line, column); }
            return Token(Token::Type::NOT, "!", line, column);
        case '<':
            if (peek() == '=') { advance(); return Token(Token::Type::LESS_EQUAL, "<=", line, column); }
            return Token(Token::Type::LESS, "<", line, column);
        case '>':
            if (peek() == '=') { advance(); return Token(Token::Type::GREATER_EQUAL, ">=", line, column); }
            return Token(Token::Type::GREATER, ">", line, column);
        case '&':
            if (peek() == '&') { advance(); return Token(Token::Type::AND, "&&", line, column); }
            return std::nullopt;
        case '|':
            if (peek() == '|') { advance(); return Token(Token::Type::OR, "||", line, column); }
            return std::nullopt;
        case '(': return Token(Token::Type::LPAREN, "(", line, column);
        case ')': return Token(Token::Type::RPAREN, ")", line, column);
        case '{': return Token(Token::Type::LBRACE, "{", line, column);
        case '}': return Token(Token::Type::RBRACE, "}", line, column);
        case '[': return Token(Token::Type::LBRACKET, "[", line, column);
        case ']': return Token(Token::Type::RBRACKET, "]", line, column);
        case ';': return Token(Token::Type::SEMICOLON, ";", line, column);
        case ':': return Token(Token::Type::COLON, ":", line, column);
        case ',': return Token(Token::Type::COMMA, ",", line, column);
        case '.': return Token(Token::Type::DOT, ".", line, column);
        default: return std::nullopt;
    }
}

// ── PARSER ──
Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), pos(0), error_count(0) {}

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
    if (peek().type == type) {
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
    std::cerr << "Parser error: " << msg << " at line " << peek().line << "\n";
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (peek().type == Token::Type::SEMICOLON) { advance(); return; }
        switch (peek().type) {
            case Token::Type::LET:
            case Token::Type::FN:
            case Token::Type::IF:
            case Token::Type::FOR:
            case Token::Type::WHILE:
            case Token::Type::RETURN:
                return;
            default: advance(); break;
        }
    }
}

int Parser::getPrecedence(Token::Type type) {
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

std::unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

std::unique_ptr<ASTNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        } else {
            synchronize();
        }
    }
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    Token tok = peek();
    
    if (tok.type == Token::Type::COMMENT) { advance(); return parseStatement(); }
    
    if (tok.type == Token::Type::LET) return parseLet();
    if (tok.type == Token::Type::IF) return parseIf();
    if (tok.type == Token::Type::WHILE) return parseWhile();
    if (tok.type == Token::Type::RETURN) return parseReturn();
    
    if (tok.value == "print" || tok.value == "println") {
        return parsePrint(tok.value == "println");
    }
    
    auto expr = parseExpression();
    if (expr) return expr;
    
    error("Unexpected token: " + tok.value);
    advance();
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    auto block = std::make_unique<BlockNode>();
    if (!match(Token::Type::LBRACE)) {
        error("Expected '{'");
        return nullptr;
    }
    while (!isAtEnd() && peek().type != Token::Type::RBRACE) {
        auto stmt = parseStatement();
        if (stmt) block->statements.push_back(std::move(stmt));
    }
    if (!match(Token::Type::RBRACE)) {
        error("Expected '}'");
        return nullptr;
    }
    return block;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseBinary(0);
}

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
        bin_op->op = op.value;
        bin_op->left = std::move(left);
        bin_op->right = std::move(right);
        left = std::move(bin_op);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    Token tok = peek();
    
    if (tok.type == Token::Type::NUMBER) {
        advance();
        auto lit = std::make_unique<LiteralNode>();
        lit->literal_type = LiteralNode::LIT_INT;
        lit->value = tok.value;
        return lit;
    }
    if (tok.type == Token::Type::STRING) {
        advance();
        auto lit = std::make_unique<LiteralNode>();
        lit->literal_type = LiteralNode::LIT_STRING;
        lit->value = tok.value;
        return lit;
    }
    if (tok.type == Token::Type::TRUE || tok.type == Token::Type::FALSE) {
        advance();
        auto lit = std::make_unique<LiteralNode>();
        lit->literal_type = LiteralNode::LIT_BOOL;
        lit->value = tok.value;
        return lit;
    }
    if (tok.type == Token::Type::IDENTIFIER) {
        advance();
        auto ident = std::make_unique<IdentifierNode>();
        ident->name = tok.value;
        return ident;
    }
    if (tok.type == Token::Type::LPAREN) {
        advance();
        auto expr = parseExpression();
        if (!match(Token::Type::RPAREN)) error("Expected ')'");
        return expr;
    }
    
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseLet() {
    advance(); // consume 'let'
    Token name = peek();
    if (name.type != Token::Type::IDENTIFIER) {
        error("Expected identifier after 'let'");
        return nullptr;
    }
    advance();
    auto node = std::make_unique<LetNode>();
    node->name = name.value;
    if (match(Token::Type::ASSIGN)) {
        node->value = parseExpression();
    }
    if (!match(Token::Type::SEMICOLON)) {
        error("Expected ';' after let");
        return nullptr;
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseIf() {
    advance(); // consume 'if'
    auto node = std::make_unique<IfNode>();
    if (!match(Token::Type::LPAREN)) { error("Expected '(' after if"); return nullptr; }
    node->condition = parseExpression();
    if (!match(Token::Type::RPAREN)) { error("Expected ')' after condition"); return nullptr; }
    node->then_branch = parseBlock();
    if (match(Token::Type::ELSE)) {
        node->else_branch = parseBlock();
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseWhile() {
    advance(); // consume 'while'
    auto node = std::make_unique<WhileNode>();
    if (!match(Token::Type::LPAREN)) { error("Expected '(' after while"); return nullptr; }
    node->condition = parseExpression();
    if (!match(Token::Type::RPAREN)) { error("Expected ')' after condition"); return nullptr; }
    node->body = parseBlock();
    return node;
}

std::unique_ptr<ASTNode> Parser::parseReturn() {
    advance(); // consume 'return'
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

std::unique_ptr<ASTNode> Parser::parsePrint(bool newline) {
    advance(); // consume 'print' or 'println'
    auto node = std::make_unique<PrintNode>(newline);
    if (!match(Token::Type::LPAREN)) { error("Expected '(' after print"); return nullptr; }
    node->value = parseExpression();
    if (!match(Token::Type::RPAREN)) { error("Expected ')' after print argument"); return nullptr; }
    if (!match(Token::Type::SEMICOLON)) { error("Expected ';' after print"); return nullptr; }
    return node;
}

} // namespace guardian::parser
