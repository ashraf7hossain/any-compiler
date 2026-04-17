.PHONY: build release clean install install-global install-deb install-rpm uninstall help test-version

COMPILER = g++
VERSION = 0.0.0

# Pass the version into the compiler as a string macro APP_VERSION
# The macro will be available in code as APP_VERSION (e.g. printf("%s", APP_VERSION);)
FLAGS = -std=c++11 -Wall -Wextra -DAPP_VERSION=\"$(VERSION)\"
RELEASE_FLAGS = -O2 -Wall -Wextra -std=c++11 -DAPP_VERSION=\"$(VERSION)\"

ifeq ($(OS),Windows_NT)
EXE_EXT = .exe
else
EXE_EXT =
endif

BINARY_BASE = any-compiler
BINARY = $(BINARY_BASE)$(EXE_EXT)
INSTALL_DIR ?= /usr/local/bin

help:
	@echo "any-compiler Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make build          - Build binary with debug symbols"
	@echo "  make release        - Build optimized release binary"
	@echo "  make clean          - Remove build artifacts"
	@echo "  make install        - Install globally for current OS"
	@echo "  make install-global - Install globally for current OS"
	@echo "  make install-deb    - Build and install .deb package"
	@echo "  make install-rpm    - Build and install .rpm package"
	@echo "  make test-version   - Test --version flag"
	@echo ""

build:
	@echo "Building (debug) with APP_VERSION=$(VERSION)"
	$(COMPILER) $(FLAGS) src/main.cpp -o $(BINARY)
	@echo "✓ Built: $(BINARY)"

release:
	@echo "Building (release) with APP_VERSION=$(VERSION)"
	$(COMPILER) $(RELEASE_FLAGS) src/main.cpp -o $(BINARY)
	@echo "✓ Release build complete: $(BINARY)"

clean:
	rm -f any-compiler any-compiler.exe
	rm -f *.deb *.rpm
	@echo "✓ Cleaned"

install: install-global

install-global:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -ExecutionPolicy Bypass -File scripts/install.ps1 -SourceDir "$(CURDIR)" -Version "$(VERSION)"
else
	bash scripts/install-source.sh "$(CURDIR)" "$(INSTALL_DIR)" "$(VERSION)"
endif

install-deb: release
	bash packaging/build-deb.sh $(VERSION)
	sudo dpkg -i any-compiler_$(VERSION)_amd64.deb
	@echo "✓ Installed via .deb"

install-rpm: release
	bash packaging/build-rpm.sh $(VERSION)
	sudo rpm -ivh any-compiler-$(VERSION)-1.x86_64.rpm
	@echo "✓ Installed via .rpm"

test-version: build
ifeq ($(OS),Windows_NT)
	.\$(BINARY) --version
	.\$(BINARY) --help
else
	./$(BINARY) --version
	./$(BINARY) --help
endif

uninstall:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -ExecutionPolicy Bypass -File scripts/install.ps1 -Uninstall
else
	sudo rm -f "$(INSTALL_DIR)/$(BINARY)"
	@echo "✓ Uninstalled $(INSTALL_DIR)/$(BINARY)"
endif
