# 📖 API Reference

Complete C++17 API documentation for The Guardian.

## Core Library (`guardian/core/`)

### Molecule

Container for quarks and atoms.

```cpp
class Molecule {
    void add_string(const std::string& name, const std::string& value);
    void add_number(const std::string& name, double value);
    void add_bool(const std::string& name, bool value);
    void add_molecule(const std::string& name, std::shared_ptr<Molecule> value);
    
    std::string get_string(const std::string& name) const;
    double get_number(const std::string& name) const;
    bool get_bool(const std::string& name) const;
    std::shared_ptr<Molecule> get_molecule(const std::string& name) const;
    
    bool has_string(const std::string& name) const;
    bool has_number(const std::string& name) const;
    bool has_bool(const std::string& name) const;
    bool has_molecule(const std::string& name) const;
    
    void remove(const std::string& name);
    void dump() const;
    size_t size() const;
};

Quark

Primitive values.
cpp

struct Quark {
    enum Type { INT, FLOAT, BOOL, STRING, NONE };
    Type type;
    
    Quark(int v);
    Quark(double v);
    Quark(bool v);
    Quark(const std::string& v);
    
    std::string to_string() const;
    size_t size() const;
};

Memory Library (guardian/memory/)
MemoryManager

Memory safety with LUT tracking.
cpp

class MemoryManager {
    void* allocate(size_t size);
    void deallocate(void* ptr);
    void register_pointer(void* ptr, size_t size, const std::string& name = "");
    void unregister_pointer(void* ptr);
    bool is_valid_pointer(void* ptr) const;
    
    struct Stats {
        size_t total_allocated;
        size_t total_freed;
        size_t current_allocations;
        size_t lut_size;
    };
    Stats get_stats() const;
    void reset();
};

Format Library (guardian/format/)
GbinFormat

Binary serialization.
cpp

class GbinFormat : public Format {
    void set_version(uint16_t version);
    uint16_t get_version() const;
    void set_data(const std::vector<uint8_t>& data);
    const std::vector<uint8_t>& get_data() const;
    
    void push_byte(uint8_t byte);
    void push_int(int value);
    void push_float(float value);
    void push_string(const std::string& str);
    
    std::vector<uint8_t> serialize();
    bool deserialize(const std::vector<uint8_t>& data);
    bool read(const std::string& path);
    bool write(const std::string& path) const;
};

VM Library (guardian/vm/)
VM

Bytecode execution engine.
cpp

class VM {
    void load(const std::vector<uint8_t>& bytecode);
    void run();
    void reset();
    bool is_running() const;
    
    void push(const Value& val);
    Value pop();
    Value peek() const;
};

