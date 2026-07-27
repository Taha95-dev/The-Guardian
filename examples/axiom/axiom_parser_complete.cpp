#include <guardian/parser/parser.hpp>
#include <guardian/vm/codegen.hpp>
#include <guardian/format/gbin_format.hpp>
#include <guardian/vm/vm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <map>
#include <vector>
#include <string>

using namespace guardian;

// ── Axiom AST Nodes ──
struct AxiomProgramNode : public parser::ProgramNode {
    // Additional Axiom-specific fields
};

struct AxiomLetNode : public parser::VariableNode {
    // Let statement
};

struct AxiomPrintNode : public parser::ASTNode {
    std::unique_ptr<parser::ASTNode> value;
    bool newline;
    AxiomPrintNode(bool nl = true) : newline(nl) { 
        type = parser::ASTNode::Type::CUSTOM; 
    }
};

// ── Axiom Parser (extends Guardian's Parser) ──
class AxiomParser : public parser::Parser {
public:
    AxiomParser(const std::vector<parser::Token>& tokens) 
        : Parser(tokens) {}

    // ── The three virtual functions ──
    std::unique_ptr<parser::ASTNode> parseExpression() override;
    std::unique_ptr<parser::ASTNode> parseStatement() override;
    std::unique_ptr<parser::ASTNode> parseBlock() override;

    // ── Root parse ──
    std::unique_ptr<parser::ASTNode> parse() override {
        auto program = std::make_unique<AxiomProgramNode>();
        while (!isAtEnd()) {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
        }
        return program;
    }

private:
    // ── Helpers ──
    std::unique_ptr<parser::ASTNode> parseLet();
    std::unique_ptr<parser::ASTNode> parsePrint(bool newline);
    std::unique_ptr<parser::ASTNode> parseBinary(int min_precedence = 0);
    std::unique_ptr<parser::ASTNode> parsePrimary();
    int getPrecedence(parser::Token::Type type);
    
    // ── Symbol table ──
    std::map<std::string, int> variables;
};

// ── Precedence Table ──
int AxiomParser::getPrecedence(parser::Token::Type type) {
    using Type = parser::Token::Type;
    switch (type) {
        case Type::OR: return 1;
        case Type::AND: return 2;
        case Type::EQUAL:
        case Type::NOT_EQUAL: return 3;
        case Type::LESS:
        case Type::GREATER:
        case Type::LESS_EQUAL:
        case Type::GREATER_EQUAL: return 4;
        case Type::PLUS:
        case Type::MINUS: return 5;
        case Type::STAR:
        case Type::SLASH:
        case Type::MOD: return 6;
        default: return 0;
    }
}

// ── parseExpression ──
std::unique_ptr<parser::ASTNode> AxiomParser::parseExpression() {
    return parseBinary(0);
}

// ── parseBinary ──
std::unique_ptr<parser::ASTNode> AxiomParser::parseBinary(int min_precedence) {
    auto left = parsePrimary();
    if (!left) return nullptr;

    while (true) {
        parser::Token op = peek();
        int prec = getPrecedence(op.type);
        if (prec < min_precedence) break;

        advance(); // consume operator

        auto right = parseBinary(prec + 1);
        if (!right) break;

        auto bin_op = std::make_unique<parser::BinaryOpNode>();
        bin_op->left = std::move(left);
        bin_op->right = std::move(right);
        bin_op->op = op.value;
        left = std::move(bin_op);
    }

    return left;
}

// ── parsePrimary ──
std::unique_ptr<parser::ASTNode> AxiomParser::parsePrimary() {
    parser::Token tok = peek();

    // Literals
    if (tok.type == parser::Token::Type::NUMBER) {
        advance();
        auto lit = std::make_unique<parser::LiteralNode>();
        lit->value = tok.value;
        return lit;
    }
    if (tok.type == parser::Token::Type::STRING) {
        advance();
        auto lit = std::make_unique<parser::LiteralNode>();
        lit->value = tok.value;
        return lit;
    }
    if (tok.type == parser::Token::Type::TRUE) {
        advance();
        auto lit = std::make_unique<parser::LiteralNode>();
        lit->value = "true";
        return lit;
    }
    if (tok.type == parser::Token::Type::FALSE) {
        advance();
        auto lit = std::make_unique<parser::LiteralNode>();
        lit->value = "false";
        return lit;
    }

    // Parenthesized expression
    if (tok.type == parser::Token::Type::LPAREN) {
        advance();
        auto expr = parseExpression();
        if (!match(parser::Token::Type::RPAREN)) {
            error("Expected ')'");
            return nullptr;
        }
        return expr;
    }

    // Variable or function call
    if (tok.type == parser::Token::Type::IDENTIFIER) {
        advance();
        if (peek().type == parser::Token::Type::LPAREN) {
            auto call = std::make_unique<parser::CallNode>();
            call->name = tok.value;
            match(parser::Token::Type::LPAREN);
            if (!match(parser::Token::Type::RPAREN)) {
                call->args.push_back(parseExpression());
                while (match(parser::Token::Type::COMMA)) {
                    call->args.push_back(parseExpression());
                }
                if (!match(parser::Token::Type::RPAREN)) {
                    error("Expected ')' after arguments");
                    return nullptr;
                }
            }
            return call;
        }
        auto var = std::make_unique<parser::VariableNode>();
        var->name = tok.value;
        return var;
    }

    error("Unexpected token in expression: " + tok.value);
    return nullptr;
}

// ── parseStatement ──
std::unique_ptr<parser::ASTNode> AxiomParser::parseStatement() {
    parser::Token tok = peek();

    if (tok.type == parser::Token::Type::KEYWORD) {
        if (tok.value == "let") {
            return parseLet();
        }
        if (tok.value == "print" || tok.value == "println") {
            return parsePrint(tok.value == "println");
        }
    }

    if (tok.type == parser::Token::Type::IF) {
        return parseIf();
    }

    if (tok.type == parser::Token::Type::FOR) {
        return parseFor();
    }

    if (tok.type == parser::Token::Type::WHILE) {
        return parseWhile();
    }

    if (tok.type == parser::Token::Type::RETURN) {
        return parseReturn();
    }

    if (tok.type == parser::Token::Type::IDENTIFIER) {
        return parseExpression();
    }

    advance();
    error("Unknown statement: " + tok.value);
    return nullptr;
}

// ── parseBlock ──
std::unique_ptr<parser::ASTNode> AxiomParser::parseBlock() {
    auto block = std::make_unique<parser::BlockNode>();

    if (!match(parser::Token::Type::LBRACE)) {
        error("Expected '{'");
        return nullptr;
    }

    while (!isAtEnd() && peek().type != parser::Token::Type::RBRACE) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        } else {
            advance();
        }
    }

    if (!match(parser::Token::Type::RBRACE)) {
        error("Expected '}'");
        return nullptr;
    }

    return block;
}

// ── parseLet ──
std::unique_ptr<parser::ASTNode> AxiomParser::parseLet() {
    advance(); // consume 'let'

    parser::Token name = peek();
    if (name.type != parser::Token::Type::IDENTIFIER) {
        error("Expected identifier after 'let'");
        return nullptr;
    }
    advance();

    auto node = std::make_unique<parser::VariableNode>();
    node->name = name.value;

    if (match(parser::Token::Type::ASSIGN)) {
        node->value = parseExpression();
        int idx = variables.size();
        variables[name.value] = idx;
        std::cout << "  " << name.value << " = " << "var[" << idx << "]\n";
    }

    if (!match(parser::Token::Type::SEMICOLON)) {
        error("Expected ';' after assignment");
        return nullptr;
    }

    return node;
}

// ── parsePrint ──
std::unique_ptr<parser::ASTNode> AxiomParser::parsePrint(bool newline) {
    advance(); // consume 'print' or 'println'

    auto node = std::make_unique<AxiomPrintNode>(newline);

    if (!match(parser::Token::Type::LPAREN)) {
        error("Expected '(' after print");
        return nullptr;
    }

    node->value = parseExpression();

    if (!match(parser::Token::Type::RPAREN)) {
        error("Expected ')' after print argument");
        return nullptr;
    }

    if (!match(parser::Token::Type::SEMICOLON)) {
        error("Expected ';' after print");
        return nullptr;
    }

    std::cout << "  " << (newline ? "println" : "print") << "\n";
    return node;
}

// ── Compiler ──
class AxiomCompiler {
public:
    bool compile(const std::string& source, const std::string& output) {
        std::cout << "🔷 Axiom Compiler (Using Guardian APIs)\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        // 1. Tokenize using Guardian's Lexer
        std::cout << "🔤 Lexing source...\n";
        parser::Lexer lexer(source);
        std::vector<parser::Token> tokens = lexer.tokenize();
        std::cout << "   Found " << tokens.size() << " tokens\n";
        
        // 2. Parse using AxiomParser
        std::cout << "📚 Parsing...\n";
        AxiomParser parser(tokens);
        auto ast = parser.parse();
        std::cout << "   AST built\n";
        
        // 3. Generate bytecode using Guardian's CodeGen
        std::cout << "⚡ Generating bytecode...\n";
        vm::CodeGen codegen;
        
        // TODO: Walk the AST and emit bytecode
        
        auto bytecode = codegen.getBytecode();
        std::cout << "   Generated " << bytecode.size() << " bytes\n";
        
        // 4. Save using Guardian's GBIN format
        std::cout << "💾 Saving to " << output << "...\n";
        format::GbinFormat gbin;
        gbin.set_version(1);
        gbin.set_data(bytecode);
        gbin.write(output);
        
        // 5. Run on Guardian's VM
        std::cout << "🧠 Running on Guardian VM...\n";
        vm::VM vm;
        vm.load(bytecode);
        vm.run();
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input.ax> <output.axbin>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    AxiomCompiler compiler;
    compiler.compile(source, argv[2]);
    
    return 0;
}
