#!/bin/bash
#
# any-compiler installer script
# Downloads and installs any-compiler globally on Linux/macOS
#
# Usage: curl -sSL https://raw.githubusercontent.com/ashraf7hossain/any-compiler/main/scripts/install.sh | bash
# Or:    wget -qO- https://raw.githubusercontent.com/ashraf7hossain/any-compiler/main/scripts/install.sh | bash

set -e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
REPO="${REPO:-ashraf7hossain/any-compiler}"
INSTALL_PATH="${INSTALL_PATH:-/usr/local/bin}"
VERSION="${VERSION:-latest}"

# Detect OS and architecture
OS=$(uname -s | tr '[:upper:]' '[:lower:]')
ARCH=$(uname -m)

case "$ARCH" in
  x86_64) ARCH="amd64" ;;
  aarch64) ARCH="arm64" ;;
  arm*) ARCH="arm" ;;
esac

echo -e "${YELLOW}Installing any-compiler...${NC}"
echo "Repository: $REPO"
echo "Install path: $INSTALL_PATH"
echo "OS: $OS, Architecture: $ARCH"

# Determine download URL
if [ "$VERSION" = "latest" ]; then
  RELEASE_INFO=$(curl -sSL "https://api.github.com/repos/$REPO/releases/latest")
  TAG=$(echo "$RELEASE_INFO" | grep '"tag_name"' | head -1 | cut -d'"' -f4)
else
  TAG="v$VERSION"
fi

DOWNLOAD_URL="https://github.com/$REPO/releases/download/$TAG/any-compiler-$OS-$ARCH"

echo "Downloading from: $DOWNLOAD_URL"

# Download binary
if ! curl -sSL "$DOWNLOAD_URL" -o /tmp/any-compiler; then
  echo -e "${RED}✗ Failed to download binary${NC}"
  echo "Please check your internet connection or visit:"
  echo "https://github.com/$REPO/releases"
  exit 1
fi

# Make executable
chmod +x /tmp/any-compiler

# Verify checksum if available
if curl -sSL "$DOWNLOAD_URL.sha256" -o /tmp/any-compiler.sha256 2>/dev/null; then
  echo -e "${YELLOW}Verifying checksum...${NC}"
  (cd /tmp && sha256sum -c any-compiler.sha256) || {
    echo -e "${RED}✗ Checksum verification failed${NC}"
    exit 1
  }
fi

# Install to system PATH
if [ -w "$INSTALL_PATH" ]; then
  mv /tmp/any-compiler "$INSTALL_PATH/any-compiler"
  echo -e "${GREEN}✓ Installed to $INSTALL_PATH/any-compiler${NC}"
else
  # Need sudo
  echo "Requires sudo to install to $INSTALL_PATH"
  sudo mv /tmp/any-compiler "$INSTALL_PATH/any-compiler"
  echo -e "${GREEN}✓ Installed to $INSTALL_PATH/any-compiler${NC}"
fi

# Verify installation
if command -v any-compiler &> /dev/null; then
  echo -e "${GREEN}✓ Installation complete!${NC}"
  echo "Version: $(any-compiler --version 2>/dev/null || echo 'unknown')"
  echo ""
  echo "Usage:"
  echo "  any-compiler <source-file>"
  echo ""
  echo "Examples:"
  echo "  any-compiler script.py"
  echo "  any-compiler program.rs"
  echo "  any-compiler app.js"
else
  echo -e "${RED}✗ Installation verification failed${NC}"
  echo "any-compiler not found in PATH"
  exit 1
fi
