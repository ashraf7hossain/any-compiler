#!/bin/bash
# Build Debian package for any-compiler

set -e

VERSION="${1:-1.0.0}"
BUILD_DIR="/tmp/any-compiler-build"

echo "Building Debian package for any-compiler v$VERSION"

# Clean and create build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/usr/local/bin"
mkdir -p "$BUILD_DIR/DEBIAN"

# Copy binary
cp any-compiler "$BUILD_DIR/usr/local/bin/"
chmod +x "$BUILD_DIR/usr/local/bin/any-compiler"

# Create control file
cat > "$BUILD_DIR/DEBIAN/control" << EOF
Package: any-compiler
Version: $VERSION
Architecture: amd64
Maintainer: Ashraf Hossain <drkownine123@gmail.com>
Homepage: https://github.com/ashraf7hossain/any-compiler
Description: Compile code in any language via OneCompiler API
 any-compiler is a command-line tool for compiling and executing code
 in multiple programming languages using the OneCompiler API.
 .
 Supported languages: C, C++, Python, JavaScript, Java, Ruby, Go, Rust, PHP, C#
Depends: curl
EOF

# Create post-install script
cat > "$BUILD_DIR/DEBIAN/postinst" << 'EOF'
#!/bin/bash
chmod +x /usr/local/bin/any-compiler
EOF
chmod +x "$BUILD_DIR/DEBIAN/postinst"

# Build deb
dpkg-deb --build "$BUILD_DIR" "any-compiler_${VERSION}_amd64.deb"

echo "✓ Package built: any-compiler_${VERSION}_amd64.deb"
