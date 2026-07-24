#include "vm.hpp"
#include "../atoms/text/string_atom.hpp"
#include <iostream>
#include <cstring>

namespace guardian::vm {

VM::VM() : running(false) {
    main_molecule = std::make_shared<guardian::Molecule>();
}

VM::~VM() = default;

void VM::load(const std::vector<uint8_t>& bytecode, size_t entry) {
    this->bytecode = bytecode;
    this->pc = entry;
    this->running = true;
    this->stack.clear();
    this->main_molecule = std::make_shared<guardian::Molecule>();
}

void VM::run() {
    while (running && pc < bytecode.size()) {
        uint8_t opcode = bytecode[pc++];
        execute(opcode);
    }
}

void VM::push(const Value& val) {
    stack.push_back(val);
}

void VM::executeMakeMolecule() {
    // Allocate struct on heap
    auto molecule = new guardian::Molecule();
    
    // Register with LUT for safety
    // main_molecule->register_pointer(molecule, "struct");
    
    // Push pointer as a value
    push(Value(molecule, Value::PtrType::STRUCT));
}

void VM::executeStoreAtom() {
    std::string name = readString();
    Value val = pop();
    
    // Get the struct pointer from the stack
    if (stack.empty()) {
        std::cerr << "Error: No struct to store atom in\n";
        running = false;
        return;
    }
    
    Value struct_val = stack.back();
    if (!struct_val.is_struct_ptr) {
        std::cerr << "Error: Not a struct pointer\n";
        running = false;
        return;
    }
    
    // Cast to Molecule*
    guardian::Molecule* molecule = static_cast<guardian::Molecule*>(struct_val.struct_ptr);
    
    // Convert value to atom
    std::shared_ptr<guardian::Atom> atom;
    if (val.is_quark) {
        switch (val.quark_data.type) {
            case guardian::QuarkType::INT:
                atom = std::make_shared<guardian::IntAtom>(val.quark_data.int_val);
                break;
            case guardian::QuarkType::FLOAT:
                atom = std::make_shared<guardian::FloatAtom>(val.quark_data.float_val);
                break;
            case guardian::QuarkType::BOOL:
                atom = std::make_shared<guardian::BoolAtom>(val.quark_data.bool_val);
                break;
            case guardian::QuarkType::CHAR: {
                std::string char_str = "char:" + std::string(1, val.quark_data.char_val);
                atom = std::make_shared<guardian::StringAtom>(char_str);
                break;
            }
            default:
                break;
        }
    } else if (val.atom_data) {
        atom = val.atom_data;
    }
    
    if (atom) {
        // Store in the struct's molecule
        molecule->add_atom_shared(name, atom);
    }
}

void VM::executeGetAtom() {
    std::string name = readString();
    
    std::cout << "  [DEBUG] GET_ATOM: getting field '" << name << "'\n";
    
    if (stack.empty()) {
        std::cerr << "Error: No struct to get atom from\n";
        running = false;
        return;
    }
    
    // The struct pointer should be on top of the stack
    Value struct_val = pop();
    
    std::cout << "  [DEBUG] GET_ATOM: struct_val.is_struct_ptr = " << struct_val.is_struct_ptr << "\n";
    
    if (!struct_val.is_struct_ptr) {
        std::cerr << "Error: Not a struct pointer\n";
        push(Value(0)); // Push placeholder
        return;
    }
    
    guardian::Molecule* molecule = static_cast<guardian::Molecule*>(struct_val.struct_ptr);
    if (!molecule) {
        std::cerr << "Error: Null struct pointer\n";
        push(Value(0));
        return;
    }
    
    auto atom = molecule->get_atom_shared(name);
    if (atom) {
        std::cout << "  [DEBUG] GET_ATOM: found atom for '" << name << "'\n";
        push(Value(atom));
    } else {
        std::cerr << "Error: Field '" << name << "' not found in struct\n";
        push(Value(0));
    }
}

Value VM::pop() {
    if (stack.empty()) {
        std::cerr << "Error: Stack underflow\n";
        return Value();
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

std::string VM::readString() {
    if (pc + 4 > bytecode.size()) {
        std::cerr << "Error: Invalid string length\n";
        return "";
    }
    uint32_t len;
    memcpy(&len, &bytecode[pc], 4);
    pc += 4;
    if (pc + len > bytecode.size()) {
        std::cerr << "Error: String extends beyond bytecode\n";
        return "";
    }
    std::string str(bytecode.begin() + pc, bytecode.begin() + pc + len);
    pc += len;
    return str;
}

void VM::setVariable(const std::string& name, const Value& value) {
    if (value.is_quark) {
        // Store quarks as atoms
        switch (value.quark_data.type) {
            case guardian::QuarkType::INT:
                main_molecule->add_number(name, value.quark_data.int_val);
                break;
            case guardian::QuarkType::FLOAT:
                main_molecule->add_number(name, value.quark_data.float_val);
                break;
            case guardian::QuarkType::FLOAT64:
                main_molecule->add_number(name, value.quark_data.float64_val);
                break;
            case guardian::QuarkType::BOOL:
                main_molecule->add_bool(name, value.quark_data.bool_val);
                break;
            case guardian::QuarkType::CHAR: {
                std::string char_str = "char:" + std::string(1, value.quark_data.char_val);
                main_molecule->add_string(name, char_str);
                break;
            }
            default:
                break;
        }
    } else if (value.is_struct_ptr) {
        // Store struct pointer as a string marker
        std::string marker = "STRUCT_PTR:" + std::to_string(reinterpret_cast<uintptr_t>(value.struct_ptr));
        main_molecule->add_string(name, marker);
        std::cout << "  [DEBUG] setVariable: stored struct pointer for " << name << "\n";
    } else if (value.is_molecule) {
        // Store molecule as a struct pointer
        // Convert molecule to a raw pointer and store it
        void* ptr = value.molecule_data.get();
        std::string marker = "STRUCT_PTR:" + std::to_string(reinterpret_cast<uintptr_t>(ptr));
        main_molecule->add_string(name, marker);
        std::cout << "  [DEBUG] setVariable: stored molecule for " << name << "\n";
    } else if (value.is_array) {
        // Store array as a JSON-like string
        std::string arr_str = "array:[";
        for (size_t i = 0; i < value.array_data.size(); i++) {
            arr_str += value.array_data[i].to_string();
            if (i < value.array_data.size() - 1) arr_str += ", ";
        }
        arr_str += "]";
        main_molecule->add_string(name, arr_str);
    } else if (value.atom_data) {
        if (auto str_atom = std::dynamic_pointer_cast<guardian::StringAtom>(value.atom_data)) {
            main_molecule->add_string(name, str_atom->get());
        }
    }
}

Value VM::getVariable(const std::string& name) const {
    std::cout << "  [DEBUG] getVariable: " << name << "\n";
    
    if (main_molecule->has_string(name)) {
        std::string value = main_molecule->get_string(name);
        
        // Check if it's a struct pointer
        if (value.rfind("STRUCT_PTR:", 0) == 0) {
            std::string addr_str = value.substr(11);
            void* ptr = reinterpret_cast<void*>(std::stoull(addr_str));
            std::cout << "  [DEBUG] getVariable: loaded struct pointer for " << name << "\n";
            return Value(ptr);
        }
        
        // Check if it's an array
        if (value.rfind("array:", 0) == 0) {
            std::cout << "  [DEBUG] Loading array: " << value << "\n";
            std::vector<Value> arr_data;
            std::string arr_str = value.substr(6);
            
            size_t pos = 1;
            while (pos < arr_str.length() && arr_str[pos] != ']') {
                while (pos < arr_str.length() && (arr_str[pos] == ' ' || arr_str[pos] == ',')) {
                    pos++;
                }
                if (pos >= arr_str.length() || arr_str[pos] == ']') break;
                
                std::string num_str;
                while (pos < arr_str.length() && arr_str[pos] != ',' && arr_str[pos] != ']') {
                    num_str += arr_str[pos++];
                }
                
                if (!num_str.empty() && num_str != "null") {
                    try {
                        int val = std::stoi(num_str);
                        arr_data.push_back(Value(val));
                    } catch (...) {
                        arr_data.push_back(Value(std::make_shared<guardian::StringAtom>(num_str)));
                    }
                } else {
                    arr_data.push_back(Value(0));
                }
            }
            
            return Value(arr_data);
        }
        
        return Value(std::make_shared<guardian::StringAtom>(value));
    }
    
    if (main_molecule->has_number(name)) {
        return Value(main_molecule->get_number(name));
    }
    
    if (main_molecule->has_bool(name)) {
        return Value(main_molecule->get_bool(name));
    }
    
    return Value(0);
}

void VM::execute(uint8_t opcode) {
    switch (static_cast<Opcode>(opcode)) {
        case Opcode::HALT:
            running = false;
            break;
            
        case Opcode::PUSH_INT: {
            if (pc + 4 > bytecode.size()) {
                std::cerr << "Error: Invalid PUSH_INT instruction\n";
                running = false;
                return;
            }
            int32_t value;
            memcpy(&value, &bytecode[pc], 4);
            pc += 4;
            push(Value(value));
            break;
        }
       
        case Opcode::MAKE_MOLECULE:
            executeMakeMolecule();
            break;
        case Opcode::STORE_ATOM:
            executeStoreAtom();
            break;
        case Opcode::GET_ATOM:
            executeGetAtom();
            break;

        case Opcode::PUSH_FLOAT: {
            if (pc + 4 > bytecode.size()) {
                std::cerr << "Error: Invalid PUSH_FLOAT instruction\n";
                running = false;
                return;
            }
            float value;
            memcpy(&value, &bytecode[pc], 4);
            pc += 4;
            push(Value(value));
            break;
        }
        
        case Opcode::PUSH_BOOL: {
            if (pc >= bytecode.size()) {
                std::cerr << "Error: Invalid PUSH_BOOL instruction\n";
                running = false;
                return;
            }
            bool value = bytecode[pc++] != 0;
            push(Value(value));
            break;
        }
       
        case Opcode::MAKE_ARRAY:
            executeMakeArray();
        break;
        case Opcode::ARRAY_GET:
            executeArrayGet();
        break;
        case Opcode::ARRAY_SET:
        executeArraySet();
        break;

        case Opcode::MOD:
            executeMod();
            break;

        case Opcode::PUSH_STRING: {
            if (pc + 4 > bytecode.size()) {
                std::cerr << "Error: Invalid PUSH_STRING instruction\n";
                running = false;
                return;
            }
            uint32_t len;
            memcpy(&len, &bytecode[pc], 4);
            pc += 4;
            if (pc + len > bytecode.size()) {
                std::cerr << "Error: String extends beyond bytecode\n";
                running = false;
                return;
            }
            std::string str(bytecode.begin() + pc, bytecode.begin() + pc + len);
            pc += len;
            auto atom = std::make_shared<guardian::StringAtom>(str);
            push(Value(atom));
            break;
        }
        
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
            
        case Opcode::PRINT:
            executePrint();
            break;
            
        case Opcode::PRINTLN:
            executePrintln();
            break;
            
        case Opcode::POP:
            pop();
            break;
            
        case Opcode::DUP: {
            if (stack.empty()) {
                std::cerr << "Error: DUP with empty stack\n";
                running = false;
                return;
            }
            push(stack.back());
            break;
        }
        
        default:
            std::cerr << "Error: Unknown opcode 0x" << std::hex << (int)opcode << "\n";
            running = false;
            break;
    }
}

void VM::executeAdd() {
    if (stack.size() < 2) {
        std::cerr << "Error: ADD requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val + right.quark_data.int_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = left.quark_data.float64_val + right.quark_data.float64_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::INT && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = (double)left.quark_data.int_val + right.quark_data.float64_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::INT) {
            double result = left.quark_data.float64_val + (double)right.quark_data.int_val;
            push(Value(result));
        } else {
            std::cerr << "Error: Unsupported ADD types\n";
            running = false;
        }
    } else {
        std::cerr << "Error: ADD currently only supports quarks\n";
        running = false;
    }
}

void VM::executeSub() {
    if (stack.size() < 2) {
        std::cerr << "Error: SUB requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val - right.quark_data.int_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = left.quark_data.float64_val - right.quark_data.float64_val;
            push(Value(result));
        } else {
            std::cerr << "Error: Unsupported SUB types\n";
            running = false;
        }
    } else {
        std::cerr << "Error: SUB currently only supports quarks\n";
        running = false;
    }
}

void VM::executeMul() {
    if (stack.size() < 2) {
        std::cerr << "Error: MUL requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val * right.quark_data.int_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = left.quark_data.float64_val * right.quark_data.float64_val;
            push(Value(result));
        } else {
            std::cerr << "Error: Unsupported MUL types\n";
            running = false;
        }
    } else {
        std::cerr << "Error: MUL currently only supports quarks\n";
        running = false;
    }
}

void VM::executeDiv() {
    if (stack.size() < 2) {
        std::cerr << "Error: DIV requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (right.is_quark && right.quark_data.type == guardian::QuarkType::INT && 
        right.quark_data.int_val == 0) {
        std::cerr << "Error: Division by zero\n";
        running = false;
        return;
    }
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val / right.quark_data.int_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = left.quark_data.float64_val / right.quark_data.float64_val;
            push(Value(result));
        } else {
            std::cerr << "Error: Unsupported DIV types\n";
            running = false;
        }
    } else {
        std::cerr << "Error: DIV currently only supports quarks\n";
        running = false;
    }
}

void VM::executeMod() {
    if (stack.size() < 2) {
        std::cerr << "Error: MOD requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val % right.quark_data.int_val;
            push(Value(result));
        } else {
            std::cerr << "Error: MOD only supports integers\n";
            running = false;
        }
    } else {
        std::cerr << "Error: MOD currently only supports quarks\n";
        running = false;
    }
}

void VM::executePrint() {
    if (stack.empty()) {
        std::cerr << "Error: PRINT with empty stack\n";
        running = false;
        return;
    }
    Value val = pop();
    std::cout << val.to_string();
}

void VM::executePrintln() {
    if (stack.empty()) {
        std::cerr << "Error: PRINTLN with empty stack\n";
        running = false;
        return;
    }
    
    Value val = pop();
    
    // Debug: see what we're printing
    std::cout << "  [DEBUG] PRINTLN: is_quark=" << val.is_quark 
              << ", is_struct_ptr=" << val.is_struct_ptr 
              << ", is_molecule=" << val.is_molecule << "\n";
    
    // If it's a struct pointer, we need to print the whole struct or a specific field
    if (val.is_struct_ptr) {
        // For now, just print the pointer address
        std::cout << "Struct at " << val.struct_ptr << "\n";
        // TODO: Print all fields of the struct
        return;
    }
    
    // If it's a molecule, print it
    if (val.is_molecule) {
        std::cout << "Molecule{...}\n";
        return;
    }
    
    // Regular printing for quarks, atoms, arrays
    std::cout << val.to_string() << "\n";
}

void VM::reset() {
    running = false;
    stack.clear();
    pc = 0;
    main_molecule = std::make_shared<guardian::Molecule>();
}

bool VM::is_running() const {
    return running;
}

void VM::executeMakeArray() {
    // Read type code from bytecode (operand)
    if (pc >= bytecode.size()) {
        std::cerr << "Error: Missing type code for MAKE_ARRAY\n";
        running = false;
        return;
    }
    
    uint8_t type_code = bytecode[pc++];
    
    // Pop size
    if (stack.empty()) {
        std::cerr << "Error: MAKE_ARRAY requires size on stack\n";
        running = false;
        return;
    }
    Value size_val = pop();
    if (!size_val.is_quark || size_val.quark_data.type != guardian::QuarkType::INT) {
        std::cerr << "Error: Array size must be an integer\n";
        running = false;
        return;
    }
    
    int size = size_val.quark_data.int_val;
    
    // Pop elements (they're on the stack in reverse order)
    std::vector<Value> arr_data;
    for (int i = 0; i < size; i++) {
        if (stack.empty()) {
            std::cerr << "Error: Not enough elements for array\n";
            running = false;
            return;
        }
        Value val = pop();
        // If it's a struct pointer, preserve it
        if (val.is_struct_ptr) {
            std::cout << "  [DEBUG] MAKE_ARRAY: storing struct pointer\n";
        }
        arr_data.insert(arr_data.begin(), val);
    }
    
    push(Value(arr_data));
}

void VM::executeArrayGet() {
    if (stack.size() < 2) {
        std::cerr << "Error: ARRAY_GET requires array and index\n";
        running = false;
        return;
    }
    
    Value index_val = pop();
    Value arr_val = pop();
    
    if (!arr_val.is_array) {
        std::cerr << "Error: ARRAY_GET requires an array\n";
        running = false;
        return;
    }
    
    if (!index_val.is_quark || index_val.quark_data.type != guardian::QuarkType::INT) {
        std::cerr << "Error: Array index must be an integer\n";
        running = false;
        return;
    }
    
    int index = index_val.quark_data.int_val;
    if (index < 0 || index >= (int)arr_val.array_data.size()) {
        std::cerr << "Error: Array index out of bounds\n";
        running = false;
        return;
    }
    
    Value result = arr_val.array_data[index];
    
    // If the result is a struct pointer, preserve it
    if (result.is_struct_ptr) {
        std::cout << "  [DEBUG] ARRAY_GET: returning struct pointer\n";
        push(result);
    } else {
        push(result);
    }
}

void VM::executeMakeDict() {
    // Pop count
    if (stack.empty()) {
        std::cerr << "Error: MAKE_DICT requires count on stack\n";
        running = false;
        return;
    }
    
    Value count_val = pop();
    if (!count_val.is_quark || count_val.quark_data.type != guardian::QuarkType::INT) {
        std::cerr << "Error: Dict count must be an integer\n";
        running = false;
        return;
    }
    
    int count = count_val.quark_data.int_val;
    auto dict = new std::unordered_map<std::string, Value>();
    
    for (int i = 0; i < count; i++) {
        Value val = pop();
        std::string key = pop().to_string();
        (*dict)[key] = val;
    }
    
    // Push dictionary pointer
    push(Value(dict, Value::PtrType::DICT));
}

void VM::executeDictGet() {
    if (stack.size() < 2) {
        std::cerr << "Error: DICT_GET requires dict and key\n";
        running = false;
        return;
    }
    
    // Pop key
    Value key_val = pop();
    // Pop dict
    Value dict_val = pop();
    
    if (!dict_val.is_dict) {
        std::cerr << "Error: DICT_GET requires a dictionary\n";
        running = false;
        return;
    }
    
    std::string key = key_val.to_string();
    
    // If dict_data has the key
    if (!dict_val.dict_data.empty()) {
        auto it = dict_val.dict_data.find(key);
        if (it != dict_val.dict_data.end()) {
            push(it->second);
            return;
        }
    }
    
    // If dict_ptr is set, use it
    if (dict_val.dict_ptr) {
        auto* dict = static_cast<std::unordered_map<std::string, Value>*>(dict_val.dict_ptr);
        auto it = dict->find(key);
        if (it != dict->end()) {
            push(it->second);
            return;
        }
    }
    
    std::cerr << "Error: Key '" << key << "' not found in dictionary\n";
    push(Value(0));
}

void VM::executeArraySet() {
    if (stack.size() < 3) {
        std::cerr << "Error: ARRAY_SET requires array, index, and value\n";
        running = false;
        return;
    }
    
    Value value = pop();
    Value index_val = pop();
    Value arr_val = pop();
    
    if (!arr_val.is_array) {
        std::cerr << "Error: ARRAY_SET requires an array\n";
        running = false;
        return;
    }
    
    if (!index_val.is_quark || index_val.quark_data.type != guardian::QuarkType::INT) {
        std::cerr << "Error: Array index must be an integer\n";
        running = false;
        return;
    }
    
    int index = index_val.quark_data.int_val;
    if (index < 0 || index >= (int)arr_val.array_data.size()) {
        std::cerr << "Error: Array index out of bounds\n";
        running = false;
        return;
    }
    
    arr_val.array_data[index] = value;
}

} // namespace guardian::vm
