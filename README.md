# 🔷 The Guardian — Language Framework

<p align="center">
  <a href="https://rad-alfajores-a2413f.netlify.app/">
    <img src="assets/banner.svg" alt="The Guardian Banner" width="100%">
  </a>
</p>

<p align="center">
  <strong>Build custom languages with memory safety, custom binary formats, and a VM.</strong><br>
  <em>Safe by Design, Not by Choice.</em>
</p>

<p align="center">
  <a href="https://rad-alfajores-a2413f.netlify.app/">
    <img src="https://img.shields.io/badge/Website-Live-667eea?style=for-the-badge&logo=netlify" alt="Website">
  </a>
  <a href="https://buy.polar.sh/polar_cl_czZ7LW2XcJR1zNjUoiGbBYU4MNOI4ShcvcR6l00HLU9">
    <img src="https://img.shields.io/badge/Buy%20Now-$20-764ba2?style=for-the-badge&logo=stripe" alt="Buy Now">
  </a>
  <a href="https://github.com/Taha95-dev/The-Guardian">
    <img src="https://img.shields.io/badge/GitHub-Repo-181717?style=for-the-badge&logo=github" alt="GitHub">
  </a>
</p>

---

## 🛒 Buy The Guardian

**The Guardian is available for commercial use.**

- 🎓 **Personal/Educational**: FREE
- 💼 **Commercial**: [Buy now on Polar ($20 one-time)](https://buy.polar.sh/polar_cl_czZ7LW2XcJR1zNjUoiGbBYU4MNOI4ShcvcR6l00HLU9)
- 🏢 **Enterprise**: Contact us for custom licensing

[![Buy on Polar](https://img.shields.io/badge/Buy%20on-Polar-667eea?style=for-the-badge)](https://buy.polar.sh/polar_cl_czZ7LW2XcJR1zNjUoiGbBYU4MNOI4ShcvcR6l00HLU9)

---

## 🌐 Website

Visit the official website: [the-guardian.netlify.app](https://rad-alfajores-a2413f.netlify.app/)

---

## Why The Guardian?

| Feature | The Guardian | Other Frameworks |
|---------|--------------|------------------|
| Custom binary formats | ✅ Built-in | ❌ |
| Memory safety (LUT) | ✅ | ❌ |
| VM execution | ✅ | ✅ |
| No Garbage Collection | ✅ | ❌ |
| Modern C++17 API | ✅ | ⚠️ |
| Nested data structures | ✅ | ⚠️ |
| System-wide installation | ✅ | ❌ |
| Built-in language demo | ✅ (Axiom) | ❌ |
| Size | ~7,000 lines | 100,000+ lines |
| Built by a 13-year-old | ✅ | ❌ |
| Price | $20 one-time | $500+ |

---

## 💰 Pricing

| License | Price | Type | Who |
|---------|-------|------|-----|
| Personal/Educational | **FREE** | One-time | Students, open-source, learning |
| Commercial | **$20** | One-time | One commercial project |
| Enterprise | **$99/year** | Subscription | Unlimited commercial projects |

[👉 Buy Commercial License ($20)](https://buy.polar.sh/polar_cl_czZ7LW2XcJR1zNjUoiGbBYU4MNOI4ShcvcR6l00HLU9)

---

## 🚀 Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/Taha95-dev/The-Guardian.git
cd The-Guardian

# Build and install
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install

# Verify installation
guardianc --version
```

#include <guardian/core/molecule.hpp>
#include <guardian/format/gbin_format.hpp>
#include <guardian/vm/vm.hpp>

using namespace guardian;

int main() {
    // Create a molecule
    Molecule mol;
    mol.add_string("greeting", "Hello from The Guardian!");
    mol.add_number("version", 1.0);
    mol.add_bool("active", true);
    
    // Serialize to binary
    format::GbinFormat fmt;
    fmt.push_string(mol.get_string("greeting"));
    fmt.write("hello.gbin");
    
    return 0;
}

📚 Documentation

    Website — Official website

    Documentation — Complete docs

    API Reference — Full API

    Examples — Axiom demo language

💻 Examples
Axiom — Complete Demo Language

Axiom is a small language built with The Guardian:
javascript

let x = 42
let name = "Axiom"
let active = true

println(x)
println(name)
println(active)

Output:
text

42
Axiom
true

Check out examples/axiom/ for the full implementation.


🔒 License

    Personal/Educational: FREE (open source)

    Commercial: $20 one-time

    Enterprise: $99/year

See LICENSE.md for full details.
👨‍💻 Built by a 13-Year-Old

The Guardian was built on:

    Hardware: HP EliteBook 840 G2 with 4GB RAM

    OS: Arch Linux

    Editor: Neovim

    Languages: C++17, Go

7,000+ lines. 0 TODOs. Built by a 13-year-old.
🤝 Contributing

Contributions are welcome! Please read CONTRIBUTING.md for guidelines.
📝 Changelog

See CHANGELOG.md for version history.
📧 Contact

    Email: kashiftaha976@gmail.com

    GitHub: Taha95-dev

    Website: rad-alfajores-a2413f.netlify.app

The Guardian — Safe by Design, Not by Choice.
