#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <map>
#include <guardian/core/molecule.hpp>
#include <guardian/format/gbin_format.hpp>
#include <guardian/vm/vm.hpp>

using namespace guardian;

// ── Token Types ──
enum class TokenType {
    LET, PRINT, IF, ELSE, WHILE, FUNC, RETURN,
    INT, FLOAT, STRING, BOOL,
    IDENTIFIER, NUMBER, STRING_LITERAL, BOOL_LITERAL,
    PLUS, MINUS, STAR, SLASH, EQUALS,
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA, COLON,
    COMMENT, EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v = "", int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}

    std::string to_string() const {
        switch (type) {
            case TokenType::LET: return "LET";
            case TokenType::PRINT: return "PRINT";
            case TokenType::IDENTIFIER: return "IDENTIFIER(" + value + ")";
            case TokenType::NUMBER: return "NUMBER(" + value + ")";
            case TokenType::STRING_LITERAL: return "STRING_LITERAL(\"" + value + "\")";
            case TokenType::BOOL_LITERAL: return "BOOL_LITERAL(" + value + ")";
            case TokenType::PLUS: return "PLUS";
            case TokenType::SEMICOLON: return "SEMICOLON";
            default: return "UNKNOWN";
        }
    }
};

// ── Lexer ──
class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;

    char peek() const {
        if (pos >= source.length()) return '\0';
        return source[pos];
    }

    char advance() {
        char c = peek();
        pos++;
        if (c == '\n') { line++; column = 1; }
        else { column++; }
        return c;
    }

    void skipWhitespace() {
        while (std::isspace(peek())) advance();
    }

    Token makeToken(TokenType type, const std::string& value = "") {
        return Token(type, value, line, column);
    }

public:
    Lexer(const std::string& src) : source(src), pos(0), line(1), column(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (true) {
            char c = peek();
            if (c == '\0') { tokens.push_back(makeToken(TokenType::EOF_TOKEN)); break; }
            if (std::isspace(c)) { skipWhitespace(); continue; }

            if (c == '/' && peekNext() == '/') {
                std::string comment;
                while (peek() != '\n' && peek() != '\0') comment += advance();
                tokens.push_back(makeToken(TokenType::COMMENT, comment));
                continue;
            }

            if (c == '"') {
                std::string str;
                advance();
                while (peek() != '"' && peek() != '\0') str += advance();
                if (peek() == '"') advance();
                tokens.push_back(makeToken(TokenType::STRING_LITERAL, str));
                continue;
            }

            if (std::isalpha(c) || c == '_') {
                std::string ident;
                while (std::isalnum(peek()) || peek() == '_') ident += advance();

                if (ident == "let") tokens.push_back(makeToken(TokenType::LET));
                else if (ident == "print") tokens.push_back(makeToken(TokenType::PRINT));
                else if (ident == "true" || ident == "false") 
                    tokens.push_back(makeToken(TokenType::BOOL_LITERAL, ident));
                else tokens.push_back(makeToken(TokenType::IDENTIFIER, ident));
                continue;
            }

            if (std::isdigit(c) || c == '.') {
                std::string num;
                bool hasDot = false;
                while (std::isdigit(peek()) || peek() == '.') {
                    if (peek() == '.') { if (hasDot) break; hasDot = true; }
                    num += advance();
                }
                tokens.push_back(makeToken(TokenType::NUMBER, num));
                continue;
            }

            switch (c) {
                case '+': advance(); tokens.push_back(makeToken(TokenType::PLUS)); break;
                case '=': advance(); tokens.push_back(makeToken(TokenType::EQUALS)); break;
                case '(': advance(); tokens.push_back(makeToken(TokenType::LPAREN)); break;
                case ')': advance(); tokens.push_back(makeToken(TokenType::RPAREN)); break;
                case ';': advance(); tokens.push_back(makeToken(TokenType::SEMICOLON)); break;
                default: advance(); break;
            }
        }
        return tokens;
    }

private:
    char peekNext() const {
        if (pos + 1 >= source.length()) return '\0';
        return source[pos + 1];
    }
};

// ── Compiler ──
class Compiler {
private:
    std::vector<Token> tokens;
    size_t pos;
    std::map<std::string, int> variables;
    std::vector<uint8_t> bytecode;
    int nextRegister = 0;

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

    void emit(uint8_t opcode, int value = 0) {
        bytecode.push_back(opcode);
        if (value != 0) {
            bytecode.push_back((value >> 24) & 0xFF);
            bytecode.push_back((value >> 16) & 0xFF);
            bytecode.push_back((value >> 8) & 0xFF);
            bytecode.push_back(value & 0xFF);
        }
    }

    void emitString(const std::string& str) {
        // Push string opcode (0x13)
        bytecode.push_back(0x13);
        // Length (2 bytes)
        uint16_t len = str.length();
        bytecode.push_back(len & 0xFF);
        bytecode.push_back((len >> 8) & 0xFF);
        // String data
        for (char c : str) {
            bytecode.push_back((uint8_t)c);
        }
    }

public:
    Compiler(const std::vector<Token>& t) : tokens(t), pos(0) {}

    std::vector<uint8_t> compile() {
        std::cout << "Compiling Axiom to bytecode...\n\n";

        while (peek().type != TokenType::EOF_TOKEN) {
            compileStatement();
        }

        // HALT
        bytecode.push_back(0x00);

        std::cout << "Compilation complete! " << bytecode.size() << " bytes generated.\n";
        return bytecode;
    }

    void compileStatement() {
        Token token = peek();

        switch (token.type) {
            case TokenType::LET:
                compileLet();
                break;
            case TokenType::PRINT:
                compilePrint();
                break;
            case TokenType::COMMENT:
                advance();
                break;
            default:
                std::cout << "  Unknown: " << token.to_string() << "\n";
                advance();
                break;
        }
    }

    void compileLet() {
        advance(); // consume 'let'
        
        Token name = advance();
        if (name.type != TokenType::IDENTIFIER) return;

        if (!match(TokenType::EQUALS)) return;

        // Compile value
        Token value = peek();
        if (value.type == TokenType::STRING_LITERAL) {
            advance();
            emitString(value.value);
            // Store in variable (STORE opcode 0x60)
            emit(0x60, variables.size());
            variables[name.value] = variables.size();
        } else if (value.type == TokenType::NUMBER) {
            advance();
            int num = std::stoi(value.value);
            emit(0x10, num); // PUSH_INT
            emit(0x60, variables.size());
            variables[name.value] = variables.size();
        } else if (value.type == TokenType::BOOL_LITERAL) {
            advance();
            bool b = (value.value == "true");
            emit(0x12, b ? 1 : 0); // PUSH_BOOL
            emit(0x60, variables.size());
            variables[name.value] = variables.size();
        }

        match(TokenType::SEMICOLON);
        std::cout << "  Compiled variable: " << name.value << "\n";
    }

    void compilePrint() {
        advance(); // consume 'print'
        match(TokenType::LPAREN);

        // Compile expression (simplified - just push the value)
        Token value = peek();
        if (value.type == TokenType::STRING_LITERAL) {
            advance();
            emitString(value.value);
        } else if (value.type == TokenType::NUMBER) {
            advance();
            int num = std::stoi(value.value);
            emit(0x10, num);
        } else if (value.type == TokenType::IDENTIFIER) {
            advance();
            // Load variable (LOAD opcode 0x61)
            auto it = variables.find(value.value);
            if (it != variables.end()) {
                emit(0x61, it->second);
            } else {
                std::cerr << "Warning: Variable '" << value.value << "' not found\n";
                emit(0x14); // PUSH_NULL
            }
        } else if (value.type == TokenType::BOOL_LITERAL) {
            advance();
            bool b = (value.value == "true");
            emit(0x12, b ? 1 : 0);
        }

        match(TokenType::RPAREN);
        match(TokenType::SEMICOLON);

        // PRINT opcode (0x80)
        bytecode.push_back(0x80);
        std::cout << "  Compiled print statement\n";
    }
};

// ── Main ──
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <file.ax> <output.axbin>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string outputfile = argv[2];

    // Read file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "🔷 Axiom Compiler\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "File: " << filename << "\n";
    std::cout << "Size: " << source.length() << " bytes\n\n";

    // Lex
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // Compile
    Compiler compiler(tokens);
    std::vector<uint8_t> bytecode = compiler.compile();

    // Save to .axbin using Guardian's GBIN format
    format::GbinFormat gbin;
    gbin.set_version(1);
    gbin.set_data(bytecode);
    gbin.write(outputfile);

    std::cout << "\n📦 Saved bytecode to: " << outputfile << "\n";
    std::cout << "   Size: " << bytecode.size() << " bytes\n";

    // Test with VM
    std::cout << "\n🧠 Testing with The Guardian VM...\n";
    vm::VM vm;
    vm.load(bytecode);
    vm.run();
    std::cout << "✅ VM execution complete!\n";

    return 0;
}
