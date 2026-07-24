# Contributing to The Guardian

Thank you for your interest in contributing to The Guardian! 🚀

This document provides guidelines and instructions for contributing to the project. By contributing, you agree that your contributions will be licensed under The Guardian License v1.0.

---

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [How to Contribute](#how-to-contribute)
4. [Development Workflow](#development-workflow)
5. [Code Style](#code-style)
6. [Testing](#testing)
7. [Documentation](#documentation)
8. [Reporting Issues](#reporting-issues)
9. [Feature Requests](#feature-requests)
10. [Pull Request Process](#pull-request-process)
11. [License](#license)
12. [Contact](#contact)

---

## Code of Conduct

### Our Pledge

We as members, contributors, and leaders pledge to make participation in our community a harassment-free experience for everyone, regardless of age, body size, visible or invisible disability, ethnicity, sex characteristics, gender identity and expression, level of experience, education, socio-economic status, nationality, personal appearance, race, caste, color, religion, or sexual identity and orientation.

### Our Standards

Examples of behavior that contributes to a positive environment:

- Using welcoming and inclusive language
- Being respectful of differing viewpoints and experiences
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

Examples of unacceptable behavior:

- The use of sexualized language or imagery
- Trolling, insulting/derogatory comments, and personal or political attacks
- Public or private harassment
- Publishing others' private information without explicit permission
- Other conduct which could reasonably be considered inappropriate

### Enforcement

Project maintainers are responsible for clarifying and enforcing these standards and may take appropriate and fair corrective action in response to any behavior they deem inappropriate, threatening, offensive, or harmful.

---

## Getting Started

### Prerequisites

- C++17 compiler (GCC 10+ or Clang 12+)
- CMake 3.10+
- Git

### Setup

```bash
# Clone the repository
git clone https://github.com/Taha95-dev/The-Guardian.git
cd The-Guardian

# Build the project
make build

# Install system-wide
sudo make install

# Verify installation
guardianc --version

How to Contribute
Types of Contributions
Type	Description
🐛 Bug Reports	Report issues you encounter
📝 Documentation	Improve docs, add examples
💡 Feature Requests	Suggest new features
🔧 Code Contributions	Fix bugs, add features
🧪 Testing	Write tests, improve coverage
🌐 Community	Help others, spread the word
Development Workflow
1. Fork the Repository

Click the Fork button on GitHub to create your own copy.
2. Clone Your Fork
bash

git clone https://github.com/your-username/The-Guardian.git
cd The-Guardian

3. Create a Branch
bash

# For new features
git checkout -b feature/your-feature-name

# For bug fixes
git checkout -b fix/your-bug-fix

# For documentation
git checkout -b docs/your-docs-update

4. Make Your Changes

Follow the Code Style guidelines.
5. Commit Your Changes
bash

git add .
git commit -m "type: brief description"

# Types:
# feat: New feature
# fix: Bug fix
# docs: Documentation
# style: Formatting
# refactor: Code restructuring
# test: Testing
# chore: Maintenance

6. Push to Your Fork
bash

git push origin your-branch-name

7. Open a Pull Request

Go to the original repository and click New Pull Request.
Code Style
C++ Standards

    Use C++17 features

    No C++20 features (yet)

    RAII for resource management

    Smart pointers over raw pointers

Formatting
cpp

// ✅ Good
class MyClass {
public:
    void myFunction();
    
private:
    int my_variable;
};

// ❌ Bad
class MyClass { public: void myFunction(); private: int my_variable; };

Naming Conventions
Type	Style	Example
Classes	PascalCase	IntAtom
Functions	camelCase	addNumber
Variables	snake_case	my_variable
Constants	UPPER_CASE	MAX_SIZE
Files	snake_case	int_atom.hpp
Comments
cpp

// ✅ Good — explains why
// Atoms are immutable for thread safety
class Atom { ... };

// ❌ Bad — states the obvious
// This is a class for atoms
class Atom { ... };

Testing
Run Tests
bash

cd build
make test
# or
./run_tests

Writing Tests
cpp

// tests/test_example.cpp
#include <iostream>

void test_example() {
    // Setup
    int expected = 42;
    
    // Execute
    int actual = myFunction();
    
    // Assert
    if (actual != expected) {
        std::cerr << "Test failed!\n";
        return;
    }
    std::cout << "Test passed!\n";
}

Documentation
Building Docs
bash

# Docs are in Markdown
# They will be available on GitHub

Documenting Code
cpp

/**
 * @brief Brief description of the function.
 *
 * Detailed description of what the function does.
 *
 * @param arg1 Description of arg1.
 * @return Description of return value.
 */
int myFunction(int arg1);

Reporting Issues
Before Submitting

    Check if the issue already exists

    Search closed issues for solutions

    Use the issue template

Issue Template
markdown

**Description**
Clear description of the issue.

**Steps to Reproduce**
1. Step 1
2. Step 2
3. Step 3

**Expected Behavior**
What should happen.

**Actual Behavior**
What actually happens.

**Environment**
- OS: Arch Linux
- Compiler: GCC 16.1.1
- Version: 0.1.0

**Additional Context**
Any other information.

Feature Requests
Before Submitting

    Check if the feature already exists

    Consider if it fits the project vision

    Use the feature request template

Feature Request Template
markdown

**Feature Description**
Clear description of the feature.

**Use Case**
How would this be used?

**Benefits**
Why is this valuable?

**Implementation Ideas**
Any ideas on how to implement this?

**Alternatives**
Any alternatives you've considered?

Pull Request Process
Before Submitting

    Ensure your code follows the style guide

    Write tests for new functionality

    Update documentation

    Run tests locally

PR Checklist

    □

    Code follows style guide
    □

    Tests added/updated
    □

    Documentation updated
    □

    Changelog updated (if needed)
    □

    All tests pass
    □

    No merge conflicts

Review Process

    At least one maintainer reviews

    Address any feedback

    Tests must pass

    PR will be merged

License

By contributing to The Guardian, you agree that your contributions will be licensed under The Guardian License v1.0.
Key Points

    ✅ You retain copyright of your contributions

    ✅ You grant the project the right to use your contributions

    ✅ Your contributions become part of the project

    ❌ You are not entitled to compensation

    ❌ You cannot revoke contributions once accepted

Contact

    GitHub Issues: github.com/Taha95-dev/The-Guardian/issues

    GitHub Discussions: github.com/Taha95-dev/The-Guardian/discussions

Recognition

Contributors will be recognized in:

    The project's README

    The CONTRIBUTORS file

    Release notes

Thank you for contributing to The Guardian! 🚀

Together, we're building the future of code.
