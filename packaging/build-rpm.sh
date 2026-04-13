#!/bin/bash
# Build RPM package for any-compiler using fpm

set -e

VERSION="${1:-1.0.0}"

echo "Building RPM package for any-compiler v$VERSION"

# Check if fpm is installed
if ! command -v fpm &> /dev/null; then
  echo "Error: fpm is not installed"
  echo "Install with: sudo gem install fpm"
  exit 1
fi

# Create temporary directory structure
BUILD_DIR="/tmp/any-compiler-rpm-build"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/usr/local/bin"

# Copy binary
cp any-compiler "$BUILD_DIR/usr/local/bin/"
chmod +x "$BUILD_DIR/usr/local/bin/any-compiler"

# Build RPM with fpm
fpm -s dir \
  -t rpm \
  -n any-compiler \
  -v "$VERSION" \
  -C "$BUILD_DIR" \
  --rpm-os linux \
  --license MIT \
  --description "Compile code in any language via OneCompiler API" \
  --url "https://github.com/ashraf7hossain/any-compiler" \
  --maintainer "Ashraf Hossain <drkownine123@gmail.com>" \
  --depends curl \
  usr/local/bin/any-compiler

echo "✓ Package built: any-compiler-${VERSION}-1.x86_64.rpm"
