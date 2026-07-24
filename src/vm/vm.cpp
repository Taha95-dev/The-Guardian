#include "vm.hpp"
#include <cctype>
#include <iostream>
#include <cstring>
#include <sstream>

namespace guardian::vm {

// ============================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================
VM::VM() : pc(0), running(false), main_molecule(std::make_shared<guardian::Molecule>()) {}

VM::~VM() {}

// ============================================
// LOAD / RUN / RESET
// ============================================
void VM::load(const std::vector<uint8_t>& code, size_t entry) {
    bytecode = code;
    pc = entry;
    running = true;
    stack.clear();
}

void VM::run() {
    while (running && pc < bytecode.size()) {
        uint8_t opcode = bytecode[pc++];
        execute(opcode);
    }
}

void VM::reset() {
    pc = 0;
    stack.clear();
    running = false;
    main_molecule = std::make_shared<guardian::Molecule>();
}

bool VM::is_running() const {
    return running;
}

// ============================================
// STACK OPERATIONS
// ============================================
void VM::push(const Value& val) {
    stack.push_back(val);
}

Value VM::pop() {
    if (stack.empty()) {
        return Value(); // Return null
    }
    Value val = stack.back();
    stack.pop_back();
    return val;
}

Value VM::peek() const {
    if (stack.empty()) {
        return Value();
    }
    return stack.back();
}

// ============================================
// HELPER FUNCTIONS FOR PARSING
// ============================================

// Forward declarations
Value parseArrayFromString(const std::string& str);
Value parseDictFromString(const std::string& str);

Value parseArrayFromString(const std::string& str) {
    std::vector<Value> array_values;
    size_t pos = 1; // Skip the opening '['
    size_t len = str.length();
    
    while (pos < len && str[pos] != ']') {
        // Skip whitespace
        while (pos < len && std::isspace(str[pos])) pos++;
        if (pos >= len || str[pos] == ']') break;
        
        // Parse the value
        if (str[pos] == '"' || str[pos] == '\'') {
            // String value
            char quote = str[pos];
            pos++;
            size_t start = pos;
            while (pos < len && str[pos] != quote) {
                if (str[pos] == '\\') pos += 2;
                else pos++;
            }
            std::string val = str.substr(start, pos - start);
            array_values.push_back(Value(val));
            if (pos < len && str[pos] == quote) pos++;
        } else if (str.compare(pos, 4, "true") == 0) {
            array_values.push_back(Value(true));
            pos += 4;
        } else if (str.compare(pos, 5, "false") == 0) {
            array_values.push_back(Value(false));
            pos += 5;
        } else if (str.compare(pos, 4, "null") == 0) {
            array_values.push_back(Value());
            pos += 4;
        } else if (std::isdigit(str[pos]) || str[pos] == '-' || str[pos] == '.') {
            // Number
            size_t start = pos;
            bool has_dot = false;
            bool has_exp = false;
            while (pos < len && (std::isdigit(str[pos]) || str[pos] == '.' || str[pos] == '-' || str[pos] == '+' || str[pos] == 'e' || str[pos] == 'E')) {
                if (str[pos] == '.') has_dot = true;
                if (str[pos] == 'e' || str[pos] == 'E') has_exp = true;
                pos++;
            }
            std::string num_str = str.substr(start, pos - start);
            if (has_dot || has_exp) {
                array_values.push_back(Value(std::stod(num_str)));
            } else {
                array_values.push_back(Value(std::stoi(num_str)));
            }
        } else if (str[pos] == '[') {
            // Nested array (recursive)
            int bracket_count = 1;
            size_t start = pos;
            pos++;
            while (pos < len && bracket_count > 0) {
                if (str[pos] == '[') bracket_count++;
                else if (str[pos] == ']') bracket_count--;
                pos++;
            }
            std::string nested = str.substr(start, pos - start);
            array_values.push_back(parseArrayFromString(nested));
        } else if (str[pos] == '{') {
            // Nested dict (recursive)
            int brace_count = 1;
            size_t start = pos;
            pos++;
            while (pos < len && brace_count > 0) {
                if (str[pos] == '{') brace_count++;
                else if (str[pos] == '}') brace_count--;
                pos++;
            }
            std::string nested = str.substr(start, pos - start);
            array_values.push_back(parseDictFromString(nested));
        }
        
        // Skip comma and whitespace
        while (pos < len && (std::isspace(str[pos]) || str[pos] == ',')) pos++;
    }
    
    return Value(array_values);
}

Value parseDictFromString(const std::string& str) {
    std::unordered_map<std::string, Value> dict_values;
    size_t pos = 1; // Skip the opening '{'
    size_t len = str.length();
    
    while (pos < len && str[pos] != '}') {
        // Skip whitespace
        while (pos < len && std::isspace(str[pos])) pos++;
        if (pos >= len || str[pos] == '}') break;
        
        // Parse key (must be a string)
        std::string key;
        if (str[pos] == '"' || str[pos] == '\'') {
            char quote = str[pos];
            pos++;
            size_t start = pos;
            while (pos < len && str[pos] != quote) {
                if (str[pos] == '\\') pos += 2;
                else pos++;
            }
            key = str.substr(start, pos - start);
            if (pos < len && str[pos] == quote) pos++;
        } else {
            // Unquoted key (simple identifier)
            size_t start = pos;
            while (pos < len && (std::isalnum(str[pos]) || str[pos] == '_')) pos++;
            key = str.substr(start, pos - start);
        }
        
        // Skip whitespace and colon
        while (pos < len && (std::isspace(str[pos]) || str[pos] == ':')) pos++;
        
        // Parse value
        Value val;
        if (pos >= len) break;
        
        if (str[pos] == '"' || str[pos] == '\'') {
            // String value
            char quote = str[pos];
            pos++;
            size_t start = pos;
            while (pos < len && str[pos] != quote) {
                if (str[pos] == '\\') pos += 2;
                else pos++;
            }
            std::string str_val = str.substr(start, pos - start);
            val = Value(str_val);
            if (pos < len && str[pos] == quote) pos++;
        } else if (str.compare(pos, 4, "true") == 0) {
            val = Value(true);
            pos += 4;
        } else if (str.compare(pos, 5, "false") == 0) {
            val = Value(false);
            pos += 5;
        } else if (str.compare(pos, 4, "null") == 0) {
            val = Value();
            pos += 4;
        } else if (std::isdigit(str[pos]) || str[pos] == '-' || str[pos] == '.') {
            // Number
            size_t start = pos;
            bool has_dot = false;
            bool has_exp = false;
            while (pos < len && (std::isdigit(str[pos]) || str[pos] == '.' || str[pos] == '-' || str[pos] == '+' || str[pos] == 'e' || str[pos] == 'E')) {
                if (str[pos] == '.') has_dot = true;
                if (str[pos] == 'e' || str[pos] == 'E') has_exp = true;
                pos++;
            }
            std::string num_str = str.substr(start, pos - start);
            if (has_dot || has_exp) {
                val = Value(std::stod(num_str));
            } else {
                val = Value(std::stoi(num_str));
            }
        } else if (str[pos] == '[') {
            // Nested array
            int bracket_count = 1;
            size_t start = pos;
            pos++;
            while (pos < len && bracket_count > 0) {
                if (str[pos] == '[') bracket_count++;
                else if (str[pos] == ']') bracket_count--;
                pos++;
            }
            std::string nested = str.substr(start, pos - start);
            val = parseArrayFromString(nested);
        } else if (str[pos] == '{') {
            // Nested dict
            int brace_count = 1;
            size_t start = pos;
            pos++;
            while (pos < len && brace_count > 0) {
                if (str[pos] == '{') brace_count++;
                else if (str[pos] == '}') brace_count--;
                pos++;
            }
            std::string nested = str.substr(start, pos - start);
            val = parseDictFromString(nested);
        }
        
        dict_values[key] = val;
        
        // Skip comma and whitespace
        while (pos < len && (std::isspace(str[pos]) || str[pos] == ',')) pos++;
    }
    
    return Value(dict_values);
}

// ============================================
// VARIABLE OPERATIONS
// ============================================
void VM::setVariable(const std::string& name, const Value& value) {
    if (value.is_quark) {
        switch (value.quark_data.type) {
            case Value::QuarkData::INT:
            case Value::QuarkData::FLOAT:
                main_molecule->add_number(name, value.quark_data.float_val);
                break;
            case Value::QuarkData::BOOL:
                main_molecule->add_bool(name, value.quark_data.bool_val);
                break;
            case Value::QuarkData::STRING:
                main_molecule->add_string(name, value.quark_data.string_val);
                break;
            default:
                break;
        }
    }
    // TODO: Handle array/dict/molecule storage if needed
}

Value VM::getVariable(const std::string& name) const {
    // Check for number (double)
    if (main_molecule->has_number(name)) {
        return Value(main_molecule->get_number(name));
    }
    
    // Check for bool
    if (main_molecule->has_bool(name)) {
        return Value(main_molecule->get_bool(name));
    }
    
    // Check for string
    if (main_molecule->has_string(name)) {
        std::string str = main_molecule->get_string(name);
        
        // Check if it's an array
        if (!str.empty() && str[0] == '[') {
            return parseArrayFromString(str);
        }
        
        // Check if it's a dict
        if (!str.empty() && str[0] == '{') {
            return parseDictFromString(str);
        }
        
        return Value(str);
    }
    
    return Value();  // Return empty/null
}

bool VM::hasVariable(const std::string& name) const {
    return main_molecule->has_number(name) ||
           main_molecule->has_bool(name) ||
           main_molecule->has_string(name);
}

// ============================================
// READ STRING FROM BYTECODE
// ============================================
std::string VM::readString() {
    if (pc >= bytecode.size()) {
        return "";
    }
    
    // Read string length (2 bytes, little-endian)
    uint16_t len = 0;
    if (pc + 1 < bytecode.size()) {
        len = bytecode[pc] | (bytecode[pc + 1] << 8);
        pc += 2;
    }
    
    if (pc + len > bytecode.size()) {
        return "";
    }
    
    std::string result(bytecode.begin() + pc, bytecode.begin() + pc + len);
    pc += len;
    return result;
}

// ============================================
// EXECUTE
// ============================================
void VM::execute(uint8_t opcode) {
    switch (static_cast<Opcode>(opcode)) {
        case Opcode::HALT:
            running = false;
            break;
            
        case Opcode::PUSH_INT: {
            if (pc + 4 <= bytecode.size()) {
                int32_t val = 0;
                val |= bytecode[pc++];
                val |= bytecode[pc++] << 8;
                val |= bytecode[pc++] << 16;
                val |= bytecode[pc++] << 24;
                push(Value(static_cast<int>(val)));
            }
            break;
        }
        
        case Opcode::REMOVE_ATOM:
            executeRemoveAtom();
            break;

        case Opcode::PUSH_FLOAT: {
            if (pc + 8 <= bytecode.size()) {
                double val;
                memcpy(&val, &bytecode[pc], sizeof(double));
                pc += 8;
                push(Value(val));
            }
            break;
        }
        
        case Opcode::PUSH_BOOL: {
            if (pc < bytecode.size()) {
                push(Value(bytecode[pc++] != 0));
            }
            break;
        }
        
        case Opcode::PUSH_STRING: {
            std::string str = readString();
            push(Value(str));
            break;
        }
        
        case Opcode::PUSH_NULL:
            push(Value());
            break;
            
        case Opcode::POP:
            pop();
            break;
            
        case Opcode::DUP: {
            Value val = peek();
            push(val);
            break;
        }
        
        case Opcode::SWAP: {
            if (stack.size() >= 2) {
                Value a = pop();
                Value b = pop();
                push(a);
                push(b);
            }
            break;
        }
        
        case Opcode::ADD:
            executeAdd();
            break;
            
        case Opcode::SUB:
            executeSub();
            break;
            
        case Opcode::MUL:
            executeMul();
            break;
            
        case Opcode::DIV:
            executeDiv();
            break;
            
        case Opcode::MOD:
            executeMod();
            break;
            
        case Opcode::PRINT:
            executePrint();
            break;
            
        case Opcode::PRINTLN:
            executePrintln();
            break;
            
        case Opcode::STORE: {
            std::string name = readString();
            Value val = pop();
            setVariable(name, val);
            break;
        }
        
        case Opcode::LOAD: {
            std::string name = readString();
            push(getVariable(name));
            break;
        }
        
        case Opcode::JMP:
            executeJmp();
            break;
            
        case Opcode::JMP_IF:
            executeJmpIf();
            break;
            
        case Opcode::JMP_IF_NOT:
            executeJmpIfNot();
            break;
            
        default:
            // Unknown opcode - halt
            running = false;
            break;
    }
}

// ============================================
// ARITHMETIC OPERATIONS
// ============================================
void VM::executeAdd() {
    Value b = pop();
    Value a = pop();
    
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            push(Value(a.quark_data.int_val + b.quark_data.int_val));
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            double result = a.quark_data.float_val + b.quark_data.float_val;
            push(Value(result));
        } else if (a.quark_data.type == Value::QuarkData::STRING) {
            push(Value(a.quark_data.string_val + b.to_string()));
        } else {
            push(Value());
        }
    } else {
        push(Value());
    }
}

void VM::executeSub() {
    Value b = pop();
    Value a = pop();
    
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            push(Value(a.quark_data.int_val - b.quark_data.int_val));
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            double result = a.quark_data.float_val - b.quark_data.float_val;
            push(Value(result));
        } else {
            push(Value());
        }
    } else {
        push(Value());
    }
}

void VM::executeMul() {
    Value b = pop();
    Value a = pop();
    
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            push(Value(a.quark_data.int_val * b.quark_data.int_val));
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            double result = a.quark_data.float_val * b.quark_data.float_val;
            push(Value(result));
        } else {
            push(Value());
        }
    } else {
        push(Value());
    }
}

void VM::executeDiv() {
    Value b = pop();
    Value a = pop();
    
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            if (b.quark_data.int_val != 0) {
                push(Value(a.quark_data.int_val / b.quark_data.int_val));
            } else {
                push(Value());
            }
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            if (b.quark_data.float_val != 0.0) {
                double result = a.quark_data.float_val / b.quark_data.float_val;
                push(Value(result));
            } else {
                push(Value());
            }
        } else {
            push(Value());
        }
    } else {
        push(Value());
    }
}

void VM::executeMod() {
    Value b = pop();
    Value a = pop();
    
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            if (b.quark_data.int_val != 0) {
                push(Value(a.quark_data.int_val % b.quark_data.int_val));
            } else {
                push(Value());
            }
        } else {
            push(Value());
        }
    } else {
        push(Value());
    }
}

// ============================================
// PRINT OPERATIONS
// ============================================
void VM::executePrint() {
    Value val = pop();
    std::cout << val.to_string();
    std::cout.flush();
}

void VM::executePrintln() {
    Value val = pop();
    std::cout << val.to_string() << std::endl;
}

// ============================================
// CONTROL FLOW
// ============================================
void VM::executeJmp() {
    if (pc + 4 <= bytecode.size()) {
        int32_t offset = 0;
        offset |= bytecode[pc++];
        offset |= bytecode[pc++] << 8;
        offset |= bytecode[pc++] << 16;
        offset |= bytecode[pc++] << 24;
        pc = static_cast<size_t>(static_cast<int>(pc) + offset - 4); // -4 because we already advanced
    }
}

void VM::executeJmpIf() {
    Value cond = pop();
    if (cond.is_quark && cond.quark_data.type == Value::QuarkData::BOOL && cond.quark_data.bool_val) {
        executeJmp();
    } else {
        // Skip the offset
        pc += 4;
    }
}

void VM::executeJmpIfNot() {
    Value cond = pop();
    if (cond.is_quark && cond.quark_data.type == Value::QuarkData::BOOL && !cond.quark_data.bool_val) {
        executeJmp();
    } else {
        // Skip the offset
        pc += 4;
    }
}

// ============================================
// ARRAY OPERATIONS (as Atoms)
// ============================================
void VM::executeMakeArray() {
    // First, count how many elements we have
    size_t count = 0;
    bool found_null = false;
    
    // Scan the stack from top to find null terminator
    for (int i = stack.size() - 1; i >= 0; i--) {
        if (stack[i].is_quark && stack[i].quark_data.type == Value::QuarkData::NONE) {
            found_null = true;
            break;
        }
        count++;
    }
    
    if (!found_null || count == 0) {
        push(Value()); // Empty array or error
        return;
    }
    
    // Now build the array by reading from stack in correct order
    std::vector<Value> elements;
    elements.reserve(count);
    
    // The first element is at stack[stack.size() - count - 1]
    size_t start_index = stack.size() - count - 1;
    for (size_t i = start_index; i < stack.size() - 1; i++) {
        elements.push_back(stack[i]);
    }
    
    // Remove all elements and the null terminator from stack
    for (size_t i = 0; i <= count; i++) {  // +1 for the null terminator
        stack.pop_back();
    }
    
    push(Value(elements));
}

void VM::executeArrayGet() {
    Value index_val = pop();
    Value array_val = pop();
    
    if (array_val.is_atom && array_val.atom_data && 
        array_val.atom_data->type == Value::AtomData::ARRAY &&
        index_val.is_quark && index_val.quark_data.type == Value::QuarkData::INT) {
        int index = index_val.quark_data.int_val;
        if (index >= 0 && static_cast<size_t>(index) < array_val.atom_data->array_data.size()) {
            push(array_val.atom_data->array_data[index]);
        } else {
            push(Value()); // Out of bounds
        }
    } else {
        push(Value());
    }
}

void VM::executeArraySet() {
    Value value = pop();
    Value index_val = pop();
    Value array_val = pop();
    
    if (array_val.is_atom && array_val.atom_data && 
        array_val.atom_data->type == Value::AtomData::ARRAY &&
        index_val.is_quark && index_val.quark_data.type == Value::QuarkData::INT) {
        int index = index_val.quark_data.int_val;
        if (index >= 0 && static_cast<size_t>(index) < array_val.atom_data->array_data.size()) {
            array_val.atom_data->array_data[index] = value;
            push(array_val); // Push back the modified atom
        } else {
            push(array_val);
        }
    } else {
        push(array_val);
    }
}

void VM::executeArrayPush() {
    Value value = pop();
    Value array_val = pop();
    
    if (array_val.is_atom && array_val.atom_data && 
        array_val.atom_data->type == Value::AtomData::ARRAY) {
        array_val.atom_data->array_data.push_back(value);
        push(array_val);
    } else {
        push(array_val);
    }
}

void VM::executeArrayPop() {
    Value array_val = pop();
    
    if (array_val.is_atom && array_val.atom_data && 
        array_val.atom_data->type == Value::AtomData::ARRAY && 
        !array_val.atom_data->array_data.empty()) {
        Value result = array_val.atom_data->array_data.back();
        array_val.atom_data->array_data.pop_back();
        push(array_val);
        push(result);
    } else {
        push(array_val);
        push(Value());
    }
}

void VM::executeArrayLen() {
    Value array_val = pop();
    
    if (array_val.is_atom && array_val.atom_data && 
        array_val.atom_data->type == Value::AtomData::ARRAY) {
        push(Value(static_cast<int>(array_val.atom_data->array_data.size())));
    } else {
        push(Value(0));
    }
}

// ============================================
// DICTIONARY OPERATIONS (as Atoms)
// ============================================
void VM::executeMakeDict() {
    // Scan stack to count entries
    size_t count = 0;
    bool found_null = false;
    
    for (int i = stack.size() - 1; i >= 0; i--) {
        if (stack[i].is_quark && stack[i].quark_data.type == Value::QuarkData::NONE) {
            found_null = true;
            break;
        }
        count++;
    }
    
    if (!found_null || count == 0) {
        push(Value()); // Empty dict or error
        return;
    }
    
    // Each entry is a pair (key, value)
    // The last two elements before null are the first key-value pair
    std::unordered_map<std::string, Value> dict;
    
    // Process from bottom to top
    size_t start_index = stack.size() - count - 1;
    for (size_t i = start_index; i < stack.size(); i += 2) {
        if (i + 1 >= stack.size()) break;
        
        Value key_val = stack[i];
        Value val = stack[i + 1];
        
        if (key_val.is_quark && key_val.quark_data.type == Value::QuarkData::STRING) {
            dict[key_val.quark_data.string_val] = val;
        }
    }
    
    // Remove all elements and the null terminator
    for (size_t i = 0; i <= count; i++) {
        stack.pop_back();
    }
    
    push(Value(dict));
}

void VM::executeDictGet() {
    Value key_val = pop();
    Value dict_val = pop();
    
    if (dict_val.is_atom && dict_val.atom_data && 
        dict_val.atom_data->type == Value::AtomData::DICT &&
        key_val.is_quark && key_val.quark_data.type == Value::QuarkData::STRING) {
        auto it = dict_val.atom_data->dict_data.find(key_val.quark_data.string_val);
        if (it != dict_val.atom_data->dict_data.end()) {
            push(it->second);
        } else {
            push(Value());
        }
    } else {
        push(Value());
    }
}

void VM::executeDictSet() {
    Value value = pop();
    Value key_val = pop();
    Value dict_val = pop();
    
    if (dict_val.is_atom && dict_val.atom_data && 
        dict_val.atom_data->type == Value::AtomData::DICT &&
        key_val.is_quark && key_val.quark_data.type == Value::QuarkData::STRING) {
        dict_val.atom_data->dict_data[key_val.quark_data.string_val] = value;
        push(dict_val);
    } else {
        push(dict_val);
    }
}

void VM::executeDictKeys() {
    Value dict_val = pop();
    
    if (dict_val.is_atom && dict_val.atom_data && 
        dict_val.atom_data->type == Value::AtomData::DICT) {
        std::vector<Value> keys;
        for (const auto& [key, _] : dict_val.atom_data->dict_data) {
            keys.push_back(Value(key));
        }
        push(Value(keys)); // Store as atom
    } else {
        push(Value());
    }
}

void VM::executeDictValues() {
    Value dict_val = pop();
    
    if (dict_val.is_atom && dict_val.atom_data && 
        dict_val.atom_data->type == Value::AtomData::DICT) {
        std::vector<Value> values;
        for (const auto& [_, val] : dict_val.atom_data->dict_data) {
            values.push_back(val);
        }
        push(Value(values)); // Store as atom
    } else {
        push(Value());
    }
}

// ============================================
// MOLECULE OPERATIONS (Container of Atoms)
// ============================================
void VM::executeMakeMolecule() {
    // Create a new molecule (container for atoms)
    auto mol = std::make_shared<guardian::Molecule>();
    push(Value(mol)); // Store as molecule atom
}

void VM::executeStoreAtom() {
    // Store an atom in a molecule by name
    Value value = pop();
    Value name_val = pop();
    Value molecule_val = pop();
    
    if (molecule_val.is_atom && molecule_val.atom_data && 
        molecule_val.atom_data->type == Value::AtomData::MOLECULE &&
        molecule_val.atom_data->molecule_data &&
        name_val.is_quark && name_val.quark_data.type == Value::QuarkData::STRING) {
        
        auto mol = molecule_val.atom_data->molecule_data;
        const std::string& name = name_val.quark_data.string_val;
        
        if (value.is_quark) {
            // Store quark as primitive
            switch (value.quark_data.type) {
                case Value::QuarkData::INT:
                case Value::QuarkData::FLOAT:
                    mol->add_number(name, value.quark_data.float_val);
                    break;
                case Value::QuarkData::BOOL:
                    mol->add_bool(name, value.quark_data.bool_val);
                    break;
                case Value::QuarkData::STRING:
                    mol->add_string(name, value.quark_data.string_val);
                    break;
                case Value::QuarkData::NONE:
                    mol->add_string(name, "null");
                    break;
            }
        } else if (value.is_atom && value.atom_data) {
            // Store atom as string representation or as special atom
            // Option 1: Store as string
            mol->add_string(name, value.to_string());
            
            // Option 2: Store as molecule atom pointer (for nested molecules)
            if (value.atom_data->type == Value::AtomData::MOLECULE && 
                value.atom_data->molecule_data) {
                // Store molecule pointer in a special way
                // You might want to add a method like add_molecule_pointer()
                // For now, store as string representation
            }
        }
        
        push(molecule_val);
    } else {
        push(Value());
    }
}

void VM::executeGetAtom() {
    // Retrieve an atom from a molecule by name
    Value name_val = pop();
    Value molecule_val = pop();
    
    if (molecule_val.is_atom && molecule_val.atom_data && 
        molecule_val.atom_data->type == Value::AtomData::MOLECULE &&
        molecule_val.atom_data->molecule_data &&
        name_val.is_quark && name_val.quark_data.type == Value::QuarkData::STRING) {
        
        auto mol = molecule_val.atom_data->molecule_data;
        const std::string& name = name_val.quark_data.string_val;
        
        // Check in order: number, bool, string
        if (mol->has_number(name)) {
            push(Value(mol->get_number(name)));
        } else if (mol->has_bool(name)) {
            push(Value(mol->get_bool(name)));
        } else if (mol->has_string(name)) {
            std::string str = mol->get_string(name);
            // Try to parse as array or dict atom
            if (!str.empty() && str[0] == '[') {
                push(parseArrayFromString(str));
            } else if (!str.empty() && str[0] == '{') {
                push(parseDictFromString(str));
            } else {
                push(Value(str));
            }
        } else {
            push(Value()); // Atom not found
        }
    } else {
        push(Value());
    }
}

void VM::executeRemoveAtom() {
    // New operation: Remove an atom from a molecule
    Value name_val = pop();
    Value molecule_val = pop();
    
    if (molecule_val.is_atom && molecule_val.atom_data && 
        molecule_val.atom_data->type == Value::AtomData::MOLECULE &&
        molecule_val.atom_data->molecule_data &&
        name_val.is_quark && name_val.quark_data.type == Value::QuarkData::STRING) {
        
        auto mol = molecule_val.atom_data->molecule_data;
        const std::string& name = name_val.quark_data.string_val;
        
        // Remove from all LUTs
        mol->remove(name);
        
        push(molecule_val);
    } else {
        push(Value());
    }
}

} // namespace guardian::vm
