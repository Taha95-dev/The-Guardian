#include "lexer.hpp"
#include <guardian/format/gbin_format.hpp>
#include <guardian/vm/vm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <memory>

using namespace axiom;

class Compiler {
public:
    Compiler(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

    std::vector<uint8_t> compile() {
        while (peek().type != TokenType::EOF_TOKEN) {
            parseStatement();
        }
        bytecode.push_back(0xFF); // HALT
        return bytecode;
    }

private:
    std::vector<Token> tokens;
    size_t pos = 0;
    std::map<std::string, int> variables;
    std::vector<uint8_t> bytecode;

    Token peek() const {
        if (pos >= tokens.size()) return Token(TokenType::EOF_TOKEN);
        return tokens[pos];
    }

    Token advance() {
        if (pos >= tokens.size()) return Token(TokenType::EOF_TOKEN);
        return tokens[pos++];
    }

    bool match(TokenType type) {
        if (peek().type == type) { advance(); return true; }
        return false;
    }

    void emit(uint8_t op) { bytecode.push_back(op); }
    void emit(uint8_t op, int val) {
        bytecode.push_back(op);
        bytecode.push_back(val & 0xFF);
        bytecode.push_back((val >> 8) & 0xFF);
        bytecode.push_back((val >> 16) & 0xFF);
        bytecode.push_back((val >> 24) & 0xFF);
    }

    void emitString(const std::string& str) {
        emit(0x13);
        uint16_t len = str.length();
        bytecode.push_back(len & 0xFF);
        bytecode.push_back((len >> 8) & 0xFF);
        for (char c : str) {
            bytecode.push_back(static_cast<uint8_t>(c));
        }
    }

    void parseStatement() {
        Token tok = peek();
        switch (tok.type) {
            case TokenType::LET: parseLet(); break;
            case TokenType::PRINT: parsePrint(false); break;
            case TokenType::PRINTLN: parsePrint(true); break;
            case TokenType::COMMENT: advance(); break;
            default: advance(); break;
        }
    }

    void parseLet() {
        advance(); // let
        Token name = advance();
        if (name.type != TokenType::IDENTIFIER) return;
        if (!match(TokenType::EQUALS)) return;

        Token value = peek();
        if (value.type == TokenType::NUMBER ||
            value.type == TokenType::STRING_LITERAL ||
            value.type == TokenType::CHAR_LITERAL ||
            value.type == TokenType::BOOL_LITERAL) {
            advance();
            pushValue(value);
            int idx = static_cast<int>(variables.size());
            variables[name.value] = idx;
            emit(0x60, idx);
            std::cout << "  " << name.value << " = " << value.value << "\n";
        }
        match(TokenType::SEMICOLON);
    }

    void parsePrint(bool newline) {
        advance(); // print or println
        match(TokenType::LPAREN);

        Token value = peek();
        if (value.type == TokenType::IDENTIFIER) {
            advance();
            auto it = variables.find(value.value);
            if (it != variables.end()) {
                emit(0x61, it->second);
            }
        } else if (value.type == TokenType::NUMBER ||
                   value.type == TokenType::STRING_LITERAL ||
                   value.type == TokenType::CHAR_LITERAL ||
                   value.type == TokenType::BOOL_LITERAL) {
            advance();
            pushValue(value);
        }

        emit(0x80);
        if (newline) {
            emit(0x83);
            std::cout << "  println\n";
        } else {
            std::cout << "  print\n";
        }

        match(TokenType::RPAREN);
        match(TokenType::SEMICOLON);
    }

    void pushValue(const Token& tok) {
        if (tok.type == TokenType::NUMBER) {
            if (tok.value.find('.') != std::string::npos) {
                emit(0x10, static_cast<int>(std::stof(tok.value)));
            } else {
                int val = std::stoi(tok.value);
                emit(0x10, val);
            }
        } else if (tok.type == TokenType::STRING_LITERAL) {
            emitString(tok.value);
        } else if (tok.type == TokenType::CHAR_LITERAL) {
            emit(0x12, static_cast<int>(tok.value[0]));
        } else if (tok.type == TokenType::BOOL_LITERAL) {
            emit(0x12, tok.value == "true" ? 1 : 0);
        }
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

    std::cout << "🔷 Axiom Compiler\n";
    std::cout << "━━━━━━━━━━━━━━━━━\n";
    std::cout << "File: " << argv[1] << "\n\n";

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Compiler compiler(tokens);
    auto bytecode = compiler.compile();

    guardian::format::GbinFormat gbin;
    gbin.set_version(1);
    gbin.set_data(bytecode);
    gbin.write(argv[2]);

    std::cout << "\n📦 Saved: " << argv[2] << " (" << bytecode.size() << " bytes)\n";

    std::cout << "\n🧠 Running on VM...\n";
    guardian::vm::VM vm;
    vm.load(bytecode);
    vm.run();
    std::cout << "✅ Done!\n";

    return 0;
}
