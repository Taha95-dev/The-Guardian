.PHONY: all build clean install uninstall test dev-install

PREFIX ?= /usr/local
BUILD_DIR = build
VERSION = 0.1.0

all: build

build:
	@echo "🔨 Building The Guardian v$(VERSION)..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_INSTALL_PREFIX=$(PREFIX) -DCMAKE_BUILD_TYPE=Release
	@cd $(BUILD_DIR) && make -j4
	@echo "✅ Build complete!"

clean:
	@echo "🧹 Cleaning..."
	@rm -rf $(BUILD_DIR)
	@rm -rf include/guardian
	@find . -name "*.o" -delete 2>/dev/null || true
	@find . -name "*.a" -delete 2>/dev/null || true
	@echo "✅ Clean complete!"

install: build
	@echo "📦 Installing to $(PREFIX)..."
	@cd $(BUILD_DIR) && sudo make install
	@echo "✅ Installation complete!"
	@echo ""
	@echo "  📁 Installed to:"
	@echo "    bin:   $(PREFIX)/bin/guardianc, $(PREFIX)/bin/omc"
	@echo "    lib:   $(PREFIX)/lib/libguardian_*.a, $(PREFIX)/lib/libom.a"
	@echo "    include: $(PREFIX)/include/guardian/"
	@echo ""

uninstall:
	@echo "🗑️  Uninstalling..."
	@sudo rm -f $(PREFIX)/bin/guardianc
	@sudo rm -f $(PREFIX)/bin/omc
	@sudo rm -f $(PREFIX)/lib/libguardian_*.a
	@sudo rm -f $(PREFIX)/lib/libom.a
	@sudo rm -rf $(PREFIX)/include/guardian
	@sudo rm -f $(PREFIX)/lib/pkgconfig/guardian.pc
	@echo "✅ Uninstall complete!"

test: build
	@echo "🧪 Running tests..."
	@cd $(BUILD_DIR) && ./run_tests

test-om: build
	@echo "🧪 Testing Om compiler..."
	@mkdir -p /tmp/om-test
	@echo 'fn main() { println("Hello from Om!"); }' > /tmp/om-test/test.om
	@$(BUILD_DIR)/omc -r /tmp/om-test/test.om
	@rm -rf /tmp/om-test
	@echo "✅ Om test complete!"

dev-install: build
	@echo "🔗 Creating development symlinks..."
	@sudo ln -sf $(PWD)/$(BUILD_DIR)/guardianc /usr/local/bin/guardianc
	@sudo ln -sf $(PWD)/$(BUILD_DIR)/omc /usr/local/bin/omc
	@echo "✅ Development symlinks created!"
	@echo "  guardianc -> $(PWD)/$(BUILD_DIR)/guardianc"
	@echo "  omc       -> $(PWD)/$(BUILD_DIR)/omc"

version:
	@echo "The Guardian v$(VERSION)"

help:
	@echo "The Guardian Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make build        - Build The Guardian"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make install      - Install system-wide (requires sudo)"
	@echo "  make uninstall    - Remove system-wide install (requires sudo)"
	@echo "  make test         - Run tests"
	@echo "  make test-om      - Test Om compiler"
	@echo "  make dev-install  - Create symlinks for development (requires sudo)"
	@echo "  make version      - Show version"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "Variables:"
	@echo "  PREFIX            - Installation prefix (default: /usr/local)"
