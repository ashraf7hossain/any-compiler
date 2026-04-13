# any-compiler

Compile and execute code in any language via the **OneCompiler API**.

## Installation

### Quick Install (Linux/macOS)

**From source:**
```bash
curl -sSL https://raw.githubusercontent.com/yourusername/any-compiler/main/scripts/install.sh | bash
```

Or manually download the [latest release](https://github.com/yourusername/any-compiler/releases).

### Homebrew (macOS/Linux)

```bash
brew tap yourusername/any-compiler
brew install any-compiler
```

### Debian/Ubuntu

```bash
sudo dpkg -i any-compiler_*.deb
```

Or add the repository:
```bash
# Coming soon
```

### From Source

```bash
git clone https://github.com/yourusername/any-compiler.git
cd any-compiler
g++ src/main.cpp -o any-compiler
sudo mv any-compiler /usr/local/bin/
```

## Usage

```bash
any-compiler <source-file>
```

### Examples

```bash
# Python
any-compiler hello.py

# Rust
any-compiler main.rs

# JavaScript
any-compiler script.js

# C++
any-compiler program.cpp

# Java
any-compiler App.java
```

## Supported Languages

- C
- C++
- Python
- JavaScript
- Java
- Ruby
- Go
- Rust
- PHP
- C#

## Features

✅ Compile and run code in any language  
✅ Real-time execution output  
✅ Error handling and reporting  
✅ JSON response parsing  
✅ Global CLI tool  

## Architecture

```
any-compiler
├── src/
│   ├── main.cpp           # Entry point
│   └── include/
│       ├── compiler.cpp   # Compilation logic
│       ├── file_helper.cpp # File I/O
│       ├── http_helper.cpp # API communication
│       └── json_helper.cpp # JSON parsing
├── .github/
│   └── workflows/
│       └── release.yml    # GitHub Actions CI/CD
├── scripts/
│   └── install.sh         # Universal installer
├── homebrew/
│   └── any-compiler.rb    # Homebrew formula
└── packaging/
    ├── build-deb.sh       # Debian packaging
    └── build-rpm.sh       # RPM packaging
```

## How It Works

1. **Read source file** - Reads the provided source code file
2. **Send to API** - Posts the code to OneCompiler API
3. **Execute** - OneCompiler compiles and runs the code
4. **Return results** - Displays stdout, stderr, and status

## Building

### Debug Build
```bash
g++ src/main.cpp -o any-compiler
```

### Release Build
```bash
g++ -O2 src/main.cpp -o any-compiler
```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## API Reference

Uses the **OneCompiler API** endpoint:
- Base URL: `https://onecompiler.com/api/code/exec`
- Method: POST
- Response format: JSON

## Troubleshooting

### Command not found

Make sure `/usr/local/bin` is in your `$PATH`:

```bash
echo $PATH
# Should include /usr/local/bin
```

### CURL errors

Ensure `curl` is installed:
```bash
sudo apt-get install curl    # Debian/Ubuntu
brew install curl             # macOS
```

### Permission denied

Make the binary executable:
```bash
chmod +x any-compiler
sudo mv any-compiler /usr/local/bin/
```

## Support

For issues and feature requests, visit the [GitHub Issues](https://github.com/yourusername/any-compiler/issues) page.

---

Made with ❤️ for developers who love simplicity.
