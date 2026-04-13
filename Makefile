.PHONY: build release clean install install-deb install-rpm help

COMPILER = g++
VERSION = 0.0.0

# Pass the version into the compiler as a string macro APP_VERSION
# The macro will be available in code as APP_VERSION (e.g. printf("%s", APP_VERSION);)
FLAGS = -std=c++11 -Wall -Wextra -DAPP_VERSION=\"$(VERSION)\"
RELEASE_FLAGS = -O2 -Wall -Wextra -std=c++11 -DAPP_VERSION=\"$(VERSION)\"

BINARY = any-compiler

help:
	@echo "any-compiler Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make build          - Build binary with debug symbols"
	@echo "  make release        - Build optimized release binary"
	@echo "  make clean          - Remove build artifacts"
	@echo "  make install        - Install to /usr/local/bin"
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
	rm -f $(BINARY)
	rm -f *.deb *.rpm
	@echo "✓ Cleaned"

install: release
	sudo cp $(BINARY) /usr/local/bin/
	@echo "✓ Installed to /usr/local/bin/$(BINARY)"

install-deb: release
	bash packaging/build-deb.sh $(VERSION)
	sudo dpkg -i any-compiler_$(VERSION)_amd64.deb
	@echo "✓ Installed via .deb"

install-rpm: release
	bash packaging/build-rpm.sh $(VERSION)
	sudo rpm -ivh any-compiler-$(VERSION)-1.x86_64.rpm
	@echo "✓ Installed via .rpm"

test-version: build
	./$(BINARY) --version
	./$(BINARY) --help

uninstall:
	sudo rm -f /usr/local/bin/$(BINARY)
	@echo "✓ Uninstalled"
