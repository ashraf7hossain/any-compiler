# any-compiler: Cross-Platform Code Compiler and Runner

**any-compiler** is a cross-platform command-line tool for compiling and running source code in multiple programming languages. It reads a local source file, sends it to the [OneCompiler API](https://onecompiler.com/), and prints the execution result. Use it on Linux, macOS, or Windows for quick code experiments without installing each language runtime locally.

Supported source languages include C, C++, C#, Go, Java, JavaScript, PHP, Python, Ruby, and Rust. An internet connection is required to send code to the OneCompiler API.

## Install

### Linux

Download the latest Linux release binary from [GitHub Releases](https://github.com/ashraf7hossain/any-compiler/releases), or install it from source. For Debian or Ubuntu, install the C++ build tools first:

```bash
sudo apt update
sudo apt install -y build-essential make git
git clone https://github.com/ashraf7hossain/any-compiler.git
cd any-compiler
make install-global VERSION=dev
```

The source installer places `any-compiler` in `/usr/local/bin` by default and may ask for `sudo` permission.

### macOS

Download the macOS binary for your processor from [GitHub Releases](https://github.com/ashraf7hossain/any-compiler/releases), or install from source. Install Apple's C++ build tools and Git if they are not already available:

```bash
xcode-select --install
git clone https://github.com/ashraf7hossain/any-compiler.git
cd any-compiler
make install-global VERSION=dev
```

The source installer places `any-compiler` in `/usr/local/bin` by default. The release page provides separate Intel (`darwin-amd64`) and Apple silicon (`darwin-arm64`) binaries.

### Windows

Download `any-compiler-windows-amd64.exe` from [GitHub Releases](https://github.com/ashraf7hossain/any-compiler/releases). To build and install from source, install Git and a MinGW-w64 `g++` toolchain, ensure `g++` is available on `PATH`, and run in PowerShell:

```powershell
git clone https://github.com/ashraf7hossain/any-compiler.git
cd any-compiler
powershell -ExecutionPolicy Bypass -File .\scripts\install.ps1 -Version dev
```

The installer puts `any-compiler.exe` in `%USERPROFILE%\.local\bin` and adds that folder to your user `PATH`. Open a new terminal after installation. Release executables use static MinGW runtime linking and do not require MinGW on the destination computer.

### Install from a release on Linux or macOS

The installer downloads the matching binary for the current operating system and CPU architecture. It installs to `/usr/local/bin` by default and may require `sudo`:

```bash
curl -fsSL https://raw.githubusercontent.com/ashraf7hossain/any-compiler/main/scripts/install.sh | bash
```

To install a specific published version, set `VERSION` (without the `v` prefix):

```bash
curl -fsSL https://raw.githubusercontent.com/ashraf7hossain/any-compiler/main/scripts/install.sh | VERSION=1.0.0 bash
```

This installer requires a matching release asset. See [GitHub Releases](https://github.com/ashraf7hossain/any-compiler/releases) for available versions and binaries.

## Run from Source

### Linux and macOS

Install `g++`, `make`, and Git, then build and run from the repository root:

```bash
git clone https://github.com/ashraf7hossain/any-compiler.git
cd any-compiler
make build
./any-compiler --version
./any-compiler src/test/test.go
```

To build an optimized binary, run `make release`. To install the locally built program globally, run `make install-global VERSION=dev`.

### Windows

With MinGW-w64 `g++` installed and available on `PATH`, build and run in PowerShell:

```powershell
git clone https://github.com/ashraf7hossain/any-compiler.git
cd any-compiler
g++ -std=c++11 -Wall -Wextra src/main.cpp -o any-compiler.exe
.\any-compiler.exe --version
.\any-compiler.exe .\src\test\test.go
```

To install the locally built program and add it to your user `PATH`, run `powershell -ExecutionPolicy Bypass -File .\scripts\install.ps1 -Version dev`.

## Develop in the VS Code Dev Container

The repository includes a VS Code Dev Container based on the official GCC image. Install Docker Desktop (or Docker Engine with Docker Compose), Visual Studio Code, and the **Dev Containers** extension. Then:

1. Clone this repository and open its folder in VS Code.
2. Open the Command Palette and select **Dev Containers: Reopen in Container**.
3. In the container terminal, build and run the CLI:

```bash
make build
./any-compiler --version
./any-compiler src/test/test.go
```

The container mounts the repository into `/usr/src/app`, so edits made in VS Code remain in your local checkout. Running a source file contacts the OneCompiler API and requires network access.

## Usage

```text
any-compiler <source-file>
```

Examples:

```bash
any-compiler hello.py
any-compiler main.go
any-compiler main.rs
any-compiler App.java
any-compiler program.cpp
```

Use `any-compiler --help` to see the command options. any-compiler sends source code to the OneCompiler API at `https://onecompiler.com/api/code/exec`; code is compiled and executed remotely, and the CLI displays the result.

## Build Requirements

- Linux and macOS: a C++11-compatible `g++` compiler; `make` is used by the provided Makefile.
- Windows: MinGW-w64 `g++` for building from source. Downloaded release binaries include the MinGW runtime statically.
- All operating systems: internet access to call the OneCompiler API.

## Contributing

Issues and pull requests are welcome. Report bugs or request features in the [GitHub issue tracker](https://github.com/ashraf7hossain/any-compiler/issues).

## License

any-compiler is distributed under the [MIT License](LICENSE).
