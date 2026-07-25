#include <guardian/parser/parser.hpp>
#include <guardian/vm/codegen.hpp>
#include <guardian/vm/vm.hpp>
#include <guardian/format/gbin_format.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

using namespace guardian;

// ── Axiom AST Nodes ──
struct AxiomProgramNode : public parser::ProgramNode {
    // Additional Axiom-specific fields
};

struct AxiomLetNode : public parser::ASTNode {
    std::string name;
    std::unique_ptr<parser::ASTNode> value;
    AxiomLetNode() { type = parser::ASTNode::Type::VARIABLE; }
};

struct AxiomPrintNode : public parser::ASTNode {
    std::unique_ptr<parser::ASTNode> value;
    bool newline;
    AxiomPrintNode(bool nl = true) : newline(nl) { 
        type = parser::ASTNode::Type::CUSTOM; 
    }
};

// ── Axiom Parser ──
class AxiomParser : public parser::Parser {
public:
    AxiomParser(const std::vector<parser::Token>& tokens) 
        : Parser(tokens) {}

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

protected:
    std::unique_ptr<parser::ASTNode> parseExpression() override {
        // Axiom expression parsing
        return nullptr;
    }

    std::unique_ptr<parser::ASTNode> parseStatement() override {
        auto tok = peek();
        
        if (tok.value == "let") {
            return parseLet();
        } else if (tok.value == "print" || tok.value == "println") {
            return parsePrint();
        }
        
        advance(); // Skip unknown
        return nullptr;
    }

    std::unique_ptr<parser::ASTNode> parseBlock() override {
        // Axiom block parsing
        return nullptr;
    }

private:
    std::unique_ptr<parser::ASTNode> parseLet() {
        advance(); // consume 'let'
        
        auto nameTok = advance();
        if (nameTok.type != parser::Token::Type::IDENTIFIER) {
            error("Expected identifier after 'let'");
            return nullptr;
        }
        
        if (!match(parser::Token::Type::ASSIGN)) {
            error("Expected '='");
            return nullptr;
        }
        
        auto node = std::make_unique<AxiomLetNode>();
        node->name = nameTok.value;
        node->value = parseExpression();
        
        match(parser::Token::Type::SEMICOLON);
        return node;
    }
    
    std::unique_ptr<parser::ASTNode> parsePrint() {
        bool newline = (peek().value == "println");
        advance(); // consume 'print' or 'println'
        
        if (!match(parser::Token::Type::LPAREN)) {
            error("Expected '('");
            return nullptr;
        }
        
        auto node = std::make_unique<AxiomPrintNode>(newline);
        node->value = parseExpression();
        
        if (!match(parser::Token::Type::RPAREN)) {
            error("Expected ')'");
            return nullptr;
        }
        
        match(parser::Token::Type::SEMICOLON);
        return node;
    }
};

// ── Axiom Compiler ──
class AxiomCompiler {
public:
    bool compile(const std::string& source, const std::string& output) {
        std::cout << "🔷 Axiom Compiler (Using Guardian APIs)\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        // 1. Tokenize using Guardian's Lexer
        std::cout << "🔤 Lexing source...\n";
        compiler::Lexer lexer(source);
        std::vector<compiler::Token> tokens = lexer.tokenize();
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
        // This is where we'd use Guardian's CodeGen API
        
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
