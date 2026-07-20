#include "parser.hpp"
#include "../core/molecule.hpp"
#include "../atoms/special/null_atom.hpp"
#include "../atoms/special/molecule_atom.hpp"
#include "../atoms/primitive/int_atom.hpp"
#include "../atoms/text/string_atom.hpp"
#include "../atoms/primitive/bool_atom.hpp"
#include <iostream>
#include <cstdlib>
#include <memory>

namespace axiom {

Parser::Parser(const std::vector<guardian::compiler::Token>& tokens) : tokens(tokens) {}

guardian::compiler::Token Parser::peek() const {
    if (pos >= tokens.size()) return guardian::compiler::Token(guardian::compiler::TokenType::END_OF_FILE, "EOF");
    return tokens[pos];
}

guardian::compiler::Token Parser::advance() {
    return tokens[pos++];
}

bool Parser::match(guardian::compiler::TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::expect(guardian::compiler::TokenType type) {
    if (peek().type != type) {
        std::cerr << "Expected token type " << static_cast<int>(type) 
                  << " but got " << static_cast<int>(peek().type) << " (" << peek().value << ")" << std::endl;
        return false;
    }
    advance();
    return true;
}

void Parser::synchronize() {
    while (peek().type != guardian::compiler::TokenType::END_OF_FILE) {
        if (peek().type == guardian::compiler::TokenType::PUNCTUATOR) {
            advance();
            return;
        }
        if (peek().type != guardian::compiler::TokenType::END_OF_FILE) {
            advance();
        }
    }
}

Builtin Parser::parse_builtin_name(const std::string& name) {
    if (name == "print") return Builtin::PRINT;
    if (name == "println") return Builtin::PRINTLN;
    if (name == "len") return Builtin::LEN;
    return Builtin::NONE;
}

std::unique_ptr<Program> Parser::parse() {
    return parse_program();
}

std::unique_ptr<Program> Parser::parse_program() {
    auto program = std::make_unique<Program>();
    
    while (peek().type != guardian::compiler::TokenType::END_OF_FILE) {
        if (peek().type == guardian::compiler::TokenType::KEYWORD && peek().value == "fn") {
            advance();
            auto func = parse_function();
            if (func) {
                program->functions.push_back(std::move(func));
            }
        } else {
            std::cerr << "Expected 'fn' at top level, got: '" << peek().value << "'" << std::endl;
            synchronize();
        }
    }
    
    return program;
}

std::unique_ptr<FunctionDef> Parser::parse_function() {
    auto func = std::make_unique<FunctionDef>();
    
    if (!expect(guardian::compiler::TokenType::IDENTIFIER)) {
        return nullptr;
    }
    func->name = tokens[pos - 1].value;
    
    if (!expect(guardian::compiler::TokenType::PUNCTUATOR)) { // (
        return nullptr;
    }
    if (!expect(guardian::compiler::TokenType::PUNCTUATOR)) { // )
        return nullptr;
    }
    
    if (!expect(guardian::compiler::TokenType::PUNCTUATOR)) { // {
        return nullptr;
    }
    
    while (peek().type != guardian::compiler::TokenType::END_OF_FILE) {
        if (peek().type == guardian::compiler::TokenType::PUNCTUATOR && peek().value == "}") {
            break;
        }
        auto stmt = parse_statement();
        if (stmt) {
            func->body.push_back(std::move(stmt));
        } else {
            synchronize();
        }
    }
    
    if (!expect(guardian::compiler::TokenType::PUNCTUATOR)) { // }
        return nullptr;
    }
    
    return func;
}

std::unique_ptr<ASTNode> Parser::parse_statement() {
    if (peek().type == guardian::compiler::TokenType::KEYWORD && peek().value == "let") {
        advance();
        return parse_let();
    }
    if (peek().type == guardian::compiler::TokenType::KEYWORD && peek().value == "return") {
        advance();
        return parse_return();
    }
    auto expr = parse_expression();
    if (!expr) {
        if (peek().type != guardian::compiler::TokenType::END_OF_FILE) {
            advance();
        }
        return nullptr;
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parse_let() {
    auto stmt = std::make_unique<LetStmt>();
    
    if (!expect(guardian::compiler::TokenType::IDENTIFIER)) {
        return nullptr;
    }
    stmt->name = tokens[pos - 1].value;
    
    if (!expect(guardian::compiler::TokenType::OPERATOR)) { // =
        return nullptr;
    }
    
    stmt->value = parse_expression();
    
    if (!expect(guardian::compiler::TokenType::PUNCTUATOR)) { // ;
        return nullptr;
    }
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parse_return() {
    auto stmt = std::make_unique<ReturnStmt>();
    stmt->value = parse_expression();
    
    if (!expect(guardian::compiler::TokenType::PUNCTUATOR)) { // ;
        return nullptr;
    }
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parse_expression() {
    auto left = parse_primary();
    if (!left) return nullptr;
    
    if (peek().type == guardian::compiler::TokenType::OPERATOR) {
        guardian::compiler::Token op = advance();
        auto right = parse_primary();
        if (!right) return nullptr;
        
        auto expr = std::make_unique<BinaryExpr>();
        expr->op = op.type;
        expr->left = std::move(left);
        expr->right = std::move(right);
        return expr;
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parse_primary() {
    auto tok = peek();
    
    if (tok.type == guardian::compiler::TokenType::NUMBER) {
        advance();
        auto expr = std::make_unique<LiteralExpr>();
        expr->type = tok.type;
        expr->value = tok.value;
        return expr;
    }
    
    if (tok.type == guardian::compiler::TokenType::STRING) {
        advance();
        auto expr = std::make_unique<LiteralExpr>();
        expr->type = tok.type;
        expr->value = tok.value;
        return expr;
    }
    
    if (tok.type == guardian::compiler::TokenType::IDENTIFIER) {
        advance();
        if (peek().type == guardian::compiler::TokenType::PUNCTUATOR && peek().value == "(") {
            return parse_call();
        }
        
        auto expr = std::make_unique<LiteralExpr>();
        expr->type = tok.type;
        expr->value = tok.value;
        return expr;
    }
    
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parse_call() {
    auto call = std::make_unique<CallExpr>();
    call->name = tokens[pos - 1].value;
    
    Builtin builtin = parse_builtin_name(call->name);
    
    if (!expect(guardian::compiler::TokenType::PUNCTUATOR)) { // (
        return nullptr;
    }
    
    while (peek().type != guardian::compiler::TokenType::END_OF_FILE) {
        if (peek().type == guardian::compiler::TokenType::PUNCTUATOR && peek().value == ")") {
            break;
        }
        auto arg = parse_expression();
        if (arg) {
            call->args.push_back(std::move(arg));
        }
        if (peek().type == guardian::compiler::TokenType::PUNCTUATOR && peek().value == ",") {
            advance();
        }
    }
    
    if (!expect(guardian::compiler::TokenType::PUNCTUATOR)) { // )
        return nullptr;
    }
    
    if (builtin != Builtin::NONE) {
        auto builtin_node = std::make_unique<BuiltinCall>();
        builtin_node->name = builtin;
        for (auto& arg : call->args) {
            builtin_node->args.push_back(std::move(arg));
        }
        return builtin_node;
    }
    
    return call;
}

// ============================================
// TO_ATOM IMPLEMENTATIONS
// ============================================

std::unique_ptr<guardian::Atom> Program::to_atom() const {
    auto mol = std::make_unique<guardian::Molecule>();
    for (const auto& func : functions) {
        mol->add_atom(func->to_atom());
    }
    return std::make_unique<guardian::MoleculeAtom>(std::move(mol));
}

std::unique_ptr<guardian::Atom> FunctionDef::to_atom() const {
    auto mol = std::make_unique<guardian::Molecule>();
    for (const auto& stmt : body) {
        mol->add_atom(stmt->to_atom());
    }
    return std::make_unique<guardian::MoleculeAtom>(std::move(mol));
}

std::unique_ptr<guardian::Atom> LetStmt::to_atom() const {
    auto mol = std::make_unique<guardian::Molecule>();
    mol->add_atom(std::make_unique<guardian::StringAtom>(name));
    mol->add_atom(value->to_atom());
    return std::make_unique<guardian::MoleculeAtom>(std::move(mol));
}

std::unique_ptr<guardian::Atom> BinaryExpr::to_atom() const {
    auto mol = std::make_unique<guardian::Molecule>();
    mol->add_atom(left->to_atom());
    mol->add_atom(std::make_unique<guardian::StringAtom>(std::to_string(static_cast<int>(op))));
    mol->add_atom(right->to_atom());
    return std::make_unique<guardian::MoleculeAtom>(std::move(mol));
}

std::unique_ptr<guardian::Atom> LiteralExpr::to_atom() const {
    if (type == guardian::compiler::TokenType::NUMBER) {
        int val = std::stoi(value);
        return std::make_unique<guardian::IntAtom>(val);
    }
    if (type == guardian::compiler::TokenType::STRING) {
        return std::make_unique<guardian::StringAtom>(value);
    }
    return std::make_unique<guardian::NullAtom>();
}

std::unique_ptr<guardian::Atom> CallExpr::to_atom() const {
    auto mol = std::make_unique<guardian::Molecule>();
    mol->add_atom(std::make_unique<guardian::StringAtom>(name));
    for (const auto& arg : args) {
        mol->add_atom(arg->to_atom());
    }
    return std::make_unique<guardian::MoleculeAtom>(std::move(mol));
}

std::unique_ptr<guardian::Atom> BuiltinCall::to_atom() const {
    auto mol = std::make_unique<guardian::Molecule>();
    mol->add_atom(std::make_unique<guardian::StringAtom>("builtin"));
    
    std::string name;
    switch (name) {
        case Builtin::PRINT: name = "print"; break;
        case Builtin::PRINTLN: name = "println"; break;
        case Builtin::LEN: name = "len"; break;
        default: name = "unknown";
    }
    mol->add_atom(std::make_unique<guardian::StringAtom>(name));
    
    for (const auto& arg : args) {
        mol->add_atom(arg->to_atom());
    }
    return std::make_unique<guardian::MoleculeAtom>(std::move(mol));
}

std::unique_ptr<guardian::Atom> ReturnStmt::to_atom() const {
    return value->to_atom();
}

} // namespace axiom
