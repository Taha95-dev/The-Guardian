#include <guardian/parser/parser.hpp>
#include <guardian/vm/codegen.hpp>
#include <guardian/format/gbin_format.hpp>
#include <guardian/vm/vm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <memory>

using namespace guardian;

// ── Axiom Parser (extends Guardian's Parser) ──
class AxiomParser : public parser::Parser {
public:
    AxiomParser(const std::vector<parser::Token>& tokens) 
        : Parser(tokens) {}

    // ── The three virtual functions ──
    std::unique_ptr<parser::ASTNode> parseExpression() override {
        // Use Guardian's parseExpression
        return parseBinary(0);
    }
    
    std::unique_ptr<parser::ASTNode> parseStatement() override {
        parser::Token tok = peek();
        
        // Use Guardian's parse functions for control flow
        if (tok.type == parser::Token::Type::IF) {
            return parseIf();
        }
        if (tok.type == parser::Token::Type::WHILE) {
            return parseWhile();
        }
        if (tok.type == parser::Token::Type::FOR) {
            return parseFor();
        }
        if (tok.type == parser::Token::Type::RETURN) {
            return parseReturn();
        }
        
        // Custom Axiom statements
        if (tok.value == "let") {
            return parseLet();
        }
        if (tok.value == "print" || tok.value == "println") {
            return parsePrint(tok.value == "println");
        }
        
        // Expression statement
        if (tok.type == parser::Token::Type::IDENTIFIER) {
            return parseExpression();
        }
        
        advance();
        return nullptr;
    }
    
    std::unique_ptr<parser::ASTNode> parseBlock() override {
        auto block = std::make_unique<parser::BlockNode>();
        
        if (!match(parser::Token::Type::LBRACE)) {
            error("Expected '{'");
            return nullptr;
        }
        
        while (!isAtEnd() && peek().type != parser::Token::Type::RBRACE) {
            auto stmt = parseStatement();
            if (stmt) {
                block->statements.push_back(std::move(stmt));
            }
        }
        
        if (!match(parser::Token::Type::RBRACE)) {
            error("Expected '}'");
            return nullptr;
        }
        
        return block;
    }

private:
    std::unique_ptr<parser::ASTNode> parseLet() {
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
        }
        
        if (!match(parser::Token::Type::SEMICOLON)) {
            error("Expected ';' after assignment");
            return nullptr;
        }
        
        return node;
    }
    
    std::unique_ptr<parser::ASTNode> parsePrint(bool newline) {
        advance(); // consume 'print' or 'println'
        auto node = std::make_unique<parser::PrintNode>(newline);
        
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
        
        return node;
    }
};

// ── Main ──
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

    std::cout << "🔷 Axiom Compiler (Using Guardian Parser)\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

    // 1. Tokenize using Guardian's Lexer
    parser::Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // 2. Parse using AxiomParser
    AxiomParser parser(tokens);
    auto ast = parser.parse();

    if (ast) {
        std::cout << "✅ AST built successfully!\n";
    } else {
        std::cerr << "❌ Parsing failed!\n";
        return 1;
    }

    // 3. Generate bytecode (placeholder for now)
    std::vector<uint8_t> bytecode;
    bytecode.push_back(0xFF); // HALT

    // 4. Save using Guardian's GBIN format
    format::GbinFormat gbin;
    gbin.set_version(1);
    gbin.set_data(bytecode);
    gbin.write(argv[2]);

    std::cout << "📦 Saved: " << argv[2] << " (" << bytecode.size() << " bytes)\n";

    // 5. Run on Guardian's VM
    std::cout << "🧠 Running on VM...\n";
    vm::VM vm;
    vm.load(bytecode);
    vm.run();
    std::cout << "✅ Done!\n";

    return 0;
}
