#pragma once

#include "lexer.hpp"
#include "ast.hpp"
#include <vector>
#include <memory>

namespace om {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    
    std::unique_ptr<ProgramNode> parse();
    
private:
    std::vector<Token> tokens;
    size_t pos;
    int error_count;
    int max_errors;
    
    bool isAtEnd() const;
    Token peek() const;
    Token advance();
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& error);
    std::string tokenTypeToString(TokenType type);
    bool isValidStatementStart();
    
    // Parsing functions
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<FunctionDefNode> parseFunctionDef();
    std::unique_ptr<VariableDefNode> parseVariableDef(bool is_const);
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseForLoop();
    std::unique_ptr<ASTNode> parseWhileLoop();
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<BlockNode> parseBlock();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseBinaryOp(int min_precedence);
    std::unique_ptr<ASTNode> parsePrimary();
    
    int getPrecedence(TokenType type);
};

} // namespace om
