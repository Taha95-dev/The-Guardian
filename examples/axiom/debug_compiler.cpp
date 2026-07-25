#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <map>
#include <cstring>
#include <guardian/core/molecule.hpp>
#include <guardian/format/gbin_format.hpp>
#include <guardian/vm/vm.hpp>

using namespace guardian;

// ... (copy the Lexer and Token code from above) ...

// ── Debug Compiler ──
class DebugCompiler {
private:
    std::vector<Token> tokens;
    size_t pos;
    std::map<std::string, int> variables;
    std::vector<uint8_t> bytecode;

    void emitInt(int value) {
        bytecode.push_back(value & 0xFF);
        bytecode.push_back((value >> 8) & 0xFF);
        bytecode.push_back((value >> 16) & 0xFF);
        bytecode.push_back((value >> 24) & 0xFF);
    }

    void emit(uint8_t opcode) {
        bytecode.push_back(opcode);
    }

    void emit(uint8_t opcode, int value) {
        bytecode.push_back(opcode);
        emitInt(value);
    }

    void emitString(const std::string& str) {
        bytecode.push_back(0x13);
        uint16_t len = str.length();
        bytecode.push_back(len & 0xFF);
        bytecode.push_back((len >> 8) & 0xFF);
        for (char c : str) {
            bytecode.push_back((uint8_t)c);
        }
    }

public:
    DebugCompiler(const std::vector<Token>& t) : tokens(t), pos(0) {}

    std::vector<uint8_t> compile() {
        std::cout << "Debug: Starting compile\n";
        
        while (pos < tokens.size()) {
            Token token = tokens[pos];
            
            if (token.type == TokenType::LET) {
                std::cout << "Debug: Found LET at pos " << pos << "\n";
                pos++; // consume let
                
                Token name = tokens[pos];
                std::cout << "Debug: Variable name: " << name.value << "\n";
                pos++; // consume identifier
                
                if (tokens[pos].type == TokenType::EQUALS) {
                    pos++; // consume =
                    std::cout << "Debug: Found '='\n";
                    
                    Token value = tokens[pos];
                    std::cout << "Debug: Value token: " << value.to_string() << "\n";
                    pos++; // consume value
                    
                    // Push value
                    if (value.type == TokenType::STRING_LITERAL) {
                        std::cout << "Debug: Emitting PUSH_STRING: " << value.value << "\n";
                        emitString(value.value);
                    } else if (value.type == TokenType::NUMBER) {
                        std::cout << "Debug: Emitting PUSH_INT: " << value.value << "\n";
                        emit(0x10, std::stoi(value.value));
                    } else if (value.type == TokenType::BOOL_LITERAL) {
                        std::cout << "Debug: Emitting PUSH_BOOL: " << value.value << "\n";
                        emit(0x12, value.value == "true" ? 1 : 0);
                    }
                    
                    // Then store
                    int varIndex = variables.size();
                    variables[name.value] = varIndex;
                    std::cout << "Debug: Emitting STORE var[" << varIndex << "]\n";
                    emit(0x60, varIndex);
                    
                    if (tokens[pos].type == TokenType::SEMICOLON) {
                        std::cout << "Debug: Found ';'\n";
                        pos++;
                    }
                }
            } else if (token.type == TokenType::PRINT) {
                std::cout << "Debug: Found PRINT at pos " << pos << "\n";
                pos++; // consume print
                
                if (tokens[pos].type == TokenType::LPAREN) {
                    pos++; // consume (
                    Token value = tokens[pos];
                    std::cout << "Debug: Print value: " << value.to_string() << "\n";
                    pos++; // consume value
                    
                    // Push value
                    if (value.type == TokenType::STRING_LITERAL) {
                        std::cout << "Debug: Emitting PUSH_STRING for print: " << value.value << "\n";
                        emitString(value.value);
                    } else if (value.type == TokenType::IDENTIFIER) {
                        std::cout << "Debug: Emitting LOAD var[" << variables[value.value] << "]\n";
                        emit(0x61, variables[value.value]);
                    } else if (value.type == TokenType::NUMBER) {
                        std::cout << "Debug: Emitting PUSH_INT for print: " << value.value << "\n";
                        emit(0x10, std::stoi(value.value));
                    } else if (value.type == TokenType::BOOL_LITERAL) {
                        std::cout << "Debug: Emitting PUSH_BOOL for print: " << value.value << "\n";
                        emit(0x12, value.value == "true" ? 1 : 0);
                    }
                    
                    // Print
                    std::cout << "Debug: Emitting PRINT\n";
                    emit(0x80);
                    
                    if (tokens[pos].type == TokenType::RPAREN) {
                        pos++;
                    }
                    if (tokens[pos].type == TokenType::SEMICOLON) {
                        pos++;
                    }
                }
            } else {
                // Skip unknown tokens
                std::cout << "Debug: Skipping token: " << token.to_string() << "\n";
                pos++;
            }
        }
        
        emit(0x00); // HALT
        return bytecode;
    }
};

// ── Main ──
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <file.ax> <output.axbin>\n";
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

    std::cout << "🔷 Axiom Debug Compiler\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "File: " << argv[1] << "\n\n";

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    DebugCompiler compiler(tokens);
    auto bytecode = compiler.compile();

    // Show bytecode
    std::cout << "\nBytecode: ";
    for (size_t i = 0; i < bytecode.size(); i++) {
        printf("%02x ", bytecode[i]);
    }
    printf("\n");

    return 0;
}
