#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <guardian/core/molecule.hpp>

using namespace guardian;

// ── Token Types ──
enum class TokenType {
    // Keywords
    LET, PRINT, IF, ELSE, WHILE, FUNC, RETURN,
    // Types
    INT, FLOAT, STRING, BOOL,
    // Literals
    IDENTIFIER, NUMBER, STRING_LITERAL, BOOL_LITERAL,
    // Operators
    PLUS, MINUS, STAR, SLASH, EQUALS,
    // Symbols
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA, COLON,
    // Other
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
            case TokenType::IF: return "IF";
            case TokenType::ELSE: return "ELSE";
            case TokenType::WHILE: return "WHILE";
            case TokenType::FUNC: return "FUNC";
            case TokenType::RETURN: return "RETURN";
            case TokenType::INT: return "INT";
            case TokenType::FLOAT: return "FLOAT";
            case TokenType::STRING: return "STRING";
            case TokenType::BOOL: return "BOOL";
            case TokenType::IDENTIFIER: return "IDENTIFIER(" + value + ")";
            case TokenType::NUMBER: return "NUMBER(" + value + ")";
            case TokenType::STRING_LITERAL: return "STRING_LITERAL(\"" + value + "\")";
            case TokenType::BOOL_LITERAL: return "BOOL_LITERAL(" + value + ")";
            case TokenType::PLUS: return "PLUS";
            case TokenType::MINUS: return "MINUS";
            case TokenType::STAR: return "STAR";
            case TokenType::SLASH: return "SLASH";
            case TokenType::EQUALS: return "EQUALS";
            case TokenType::LPAREN: return "LPAREN";
            case TokenType::RPAREN: return "RPAREN";
            case TokenType::LBRACE: return "LBRACE";
            case TokenType::RBRACE: return "RBRACE";
            case TokenType::SEMICOLON: return "SEMICOLON";
            case TokenType::COMMA: return "COMMA";
            case TokenType::COLON: return "COLON";
            case TokenType::COMMENT: return "COMMENT(" + value + ")";
            case TokenType::EOF_TOKEN: return "EOF";
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
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        return c;
    }

    void skipWhitespace() {
        while (std::isspace(peek())) {
            advance();
        }
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

            if (c == '\0') {
                tokens.push_back(makeToken(TokenType::EOF_TOKEN));
                break;
            }

            // Skip whitespace
            if (std::isspace(c)) {
                skipWhitespace();
                continue;
            }

            // Comments
            if (c == '/' && peekNext() == '/') {
                std::string comment;
                while (peek() != '\n' && peek() != '\0') {
                    comment += advance();
                }
                tokens.push_back(makeToken(TokenType::COMMENT, comment));
                continue;
            }

            // Strings
            if (c == '"') {
                std::string str;
                advance(); // skip opening "
                while (peek() != '"' && peek() != '\0') {
                    str += advance();
                }
                if (peek() == '"') advance(); // skip closing "
                tokens.push_back(makeToken(TokenType::STRING_LITERAL, str));
                continue;
            }

            // Identifiers and keywords
            if (std::isalpha(c) || c == '_') {
                std::string ident;
                while (std::isalnum(peek()) || peek() == '_') {
                    ident += advance();
                }

                // Check keywords
                if (ident == "let") {
                    tokens.push_back(makeToken(TokenType::LET));
                } else if (ident == "print") {
                    tokens.push_back(makeToken(TokenType::PRINT));
                } else if (ident == "if") {
                    tokens.push_back(makeToken(TokenType::IF));
                } else if (ident == "else") {
                    tokens.push_back(makeToken(TokenType::ELSE));
                } else if (ident == "while") {
                    tokens.push_back(makeToken(TokenType::WHILE));
                } else if (ident == "func") {
                    tokens.push_back(makeToken(TokenType::FUNC));
                } else if (ident == "return") {
                    tokens.push_back(makeToken(TokenType::RETURN));
                } else if (ident == "int") {
                    tokens.push_back(makeToken(TokenType::INT));
                } else if (ident == "float") {
                    tokens.push_back(makeToken(TokenType::FLOAT));
                } else if (ident == "string") {
                    tokens.push_back(makeToken(TokenType::STRING));
                } else if (ident == "bool") {
                    tokens.push_back(makeToken(TokenType::BOOL));
                } else if (ident == "true" || ident == "false") {
                    tokens.push_back(makeToken(TokenType::BOOL_LITERAL, ident));
                } else {
                    tokens.push_back(makeToken(TokenType::IDENTIFIER, ident));
                }
                continue;
            }

            // Numbers (including floats)
            if (std::isdigit(c) || c == '.') {
                std::string num;
                bool hasDot = false;
                
                while (std::isdigit(peek()) || peek() == '.') {
                    if (peek() == '.') {
                        if (hasDot) break; // Second dot is error
                        hasDot = true;
                    }
                    num += advance();
                }
                
                tokens.push_back(makeToken(TokenType::NUMBER, num));
                continue;
            }

            // Single-character tokens
            switch (c) {
                case '+': advance(); tokens.push_back(makeToken(TokenType::PLUS)); break;
                case '-': advance(); tokens.push_back(makeToken(TokenType::MINUS)); break;
                case '*': advance(); tokens.push_back(makeToken(TokenType::STAR)); break;
                case '/': advance(); tokens.push_back(makeToken(TokenType::SLASH)); break;
                case '=': advance(); tokens.push_back(makeToken(TokenType::EQUALS)); break;
                case '(': advance(); tokens.push_back(makeToken(TokenType::LPAREN)); break;
                case ')': advance(); tokens.push_back(makeToken(TokenType::RPAREN)); break;
                case '{': advance(); tokens.push_back(makeToken(TokenType::LBRACE)); break;
                case '}': advance(); tokens.push_back(makeToken(TokenType::RBRACE)); break;
                case ';': advance(); tokens.push_back(makeToken(TokenType::SEMICOLON)); break;
                case ',': advance(); tokens.push_back(makeToken(TokenType::COMMA)); break;
                case ':': advance(); tokens.push_back(makeToken(TokenType::COLON)); break;
                default:
                    std::cerr << "Unknown character: " << c << " at line " << line << "\n";
                    advance();
                    break;
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

// ── Parser ──
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;

    Token peek() const {
        if (pos >= tokens.size()) return Token(TokenType::EOF_TOKEN);
        return tokens[pos];
    }

    Token advance() {
        if (pos >= tokens.size()) return Token(TokenType::EOF_TOKEN);
        return tokens[pos++];
    }

    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    bool expect(TokenType type, const std::string& error_msg) {
        if (peek().type == type) {
            advance();
            return true;
        }
        std::cerr << "Error: " << error_msg << " at line " << peek().line << "\n";
        return false;
    }

public:
    Parser(const std::vector<Token>& t) : tokens(t), pos(0) {}

    void parse() {
        std::cout << "Parsing Axiom program...\n\n";

        while (peek().type != TokenType::EOF_TOKEN) {
            parseStatement();
        }

        std::cout << "\nParsing complete!\n";
    }

    void parseStatement() {
        Token token = peek();

        switch (token.type) {
            case TokenType::LET:
                parseLetStatement();
                break;
            case TokenType::PRINT:
                parsePrintStatement();
                break;
            case TokenType::COMMENT:
                std::cout << "  Comment: " << token.value << "\n";
                advance();
                break;
            default:
                std::cout << "  Unknown statement: " << token.to_string() << "\n";
                advance();
                break;
        }
    }

    void parseLetStatement() {
        advance(); // consume 'let'
        
        Token name = advance();
        if (name.type != TokenType::IDENTIFIER) {
            std::cerr << "Expected identifier after 'let'\n";
            return;
        }

        if (!expect(TokenType::EQUALS, "Expected '=' after identifier")) return;

        // Parse value
        std::string value_str;
        Token value = peek();
        
        // Handle different value types
        if (value.type == TokenType::STRING_LITERAL) {
            value_str = "\"" + value.value + "\"";
            advance();
        } else if (value.type == TokenType::NUMBER) {
            value_str = value.value;
            advance();
        } else if (value.type == TokenType::BOOL_LITERAL) {
            value_str = value.value;
            advance();
        } else if (value.type == TokenType::IDENTIFIER) {
            value_str = value.value;
            advance();
        } else {
            std::cerr << "Unexpected value type: " << value.to_string() << "\n";
            return;
        }

        std::cout << "  Variable: " << name.value << " = " << value_str << "\n";

        if (!expect(TokenType::SEMICOLON, "Expected ';' after statement")) {
            // If we don't see a semicolon, try to advance to find one
            while (peek().type != TokenType::SEMICOLON && peek().type != TokenType::EOF_TOKEN) {
                advance();
            }
            if (peek().type == TokenType::SEMICOLON) {
                advance();
            }
        }
    }

    void parsePrintStatement() {
        advance(); // consume 'print'

        if (!expect(TokenType::LPAREN, "Expected '(' after print")) return;

        // Parse expression
        std::string expr;
        int parenCount = 1;
        
        while (parenCount > 0 && peek().type != TokenType::EOF_TOKEN) {
            Token t = peek();
            
            if (t.type == TokenType::LPAREN) {
                parenCount++;
            } else if (t.type == TokenType::RPAREN) {
                parenCount--;
                if (parenCount == 0) {
                    advance();
                    break;
                }
            }
            
            if (parenCount > 0) {
                expr += t.to_string() + " ";
                advance();
            }
        }

        std::cout << "  Print: " << expr << "\n";

        if (peek().type == TokenType::SEMICOLON) {
            advance();
        } else {
            std::cerr << "Warning: Expected ';' after print at line " << peek().line << "\n";
        }
    }
};

// ── Main ──
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.ax>\n";
        return 1;
    }

    std::string filename = argv[1];

    // Read file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "📜 Axiom Language Parser\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "File: " << filename << "\n";
    std::cout << "Size: " << source.length() << " bytes\n\n";

    // Lex
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "🔤 Tokens:\n";
    for (const auto& token : tokens) {
        if (token.type != TokenType::EOF_TOKEN) {
            std::cout << "  " << token.to_string() << "\n";
        }
    }
    std::cout << "\n";

    // Parse
    Parser parser(tokens);
    parser.parse();

    // Use The Guardian (show Molecule)
    std::cout << "\n🧬 Using The Guardian:\n";
    Molecule mol;
    mol.add_string("language", "Axiom");
    mol.add_number("version", 1.0);
    mol.add_bool("complete", false);
    mol.dump();

    return 0;
}
