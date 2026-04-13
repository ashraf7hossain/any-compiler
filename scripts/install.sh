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
  RELEASE_INFO=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/latest" || true)
  TAG=$(printf "%s" "$RELEASE_INFO" | grep -oE '"tag_name"[[:space:]]*:[[:space:]]*"[^"]+"' | head -1 | cut -d'"' -f4)
  if [ -z "$TAG" ]; then
    echo -e "${RED}✗ Could not determine latest release tag${NC}"
    echo "Make sure at least one GitHub Release exists, or provide VERSION explicitly."
    echo "Example: VERSION=1.0.0 curl -sSL https://raw.githubusercontent.com/$REPO/main/scripts/install.sh | bash"
    exit 1
  fi
else
  if [[ "$VERSION" == v* ]]; then
    TAG="$VERSION"
  else
    TAG="v$VERSION"
  fi
fi

DOWNLOAD_URL="https://github.com/$REPO/releases/download/$TAG/any-compiler-$OS-$ARCH"

echo "Downloading from: $DOWNLOAD_URL"

# Download binary
if ! curl -fsSL "$DOWNLOAD_URL" -o /tmp/any-compiler; then
  echo -e "${RED}✗ Failed to download binary${NC}"
  echo "Please check your internet connection or visit:"
  echo "https://github.com/$REPO/releases"
  exit 1
fi

# Make executable
chmod +x /tmp/any-compiler

# Verify checksum if available
if curl -fsSL "$DOWNLOAD_URL.sha256" -o /tmp/any-compiler.sha256 2>/dev/null; then
  echo -e "${YELLOW}Verifying checksum...${NC}"
  if grep -Eq '^[a-fA-F0-9]{64}[[:space:]]+\*?any-compiler$' /tmp/any-compiler.sha256; then
    (cd /tmp && sha256sum -c any-compiler.sha256) || {
      echo -e "${RED}✗ Checksum verification failed${NC}"
      exit 1
    }
  elif grep -Eq '^[a-fA-F0-9]{64}$' /tmp/any-compiler.sha256; then
    EXPECTED_SUM=$(cat /tmp/any-compiler.sha256)
    ACTUAL_SUM=$(sha256sum /tmp/any-compiler | awk '{print $1}')
    if [ "$EXPECTED_SUM" != "$ACTUAL_SUM" ]; then
      echo -e "${RED}✗ Checksum verification failed${NC}"
      exit 1
    fi
  else
    echo -e "${YELLOW}Checksum file format not recognized, skipping verification${NC}"
  fi
else
  echo -e "${YELLOW}No checksum file found for this release asset, skipping verification${NC}"
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
