#!/bin/bash
#
# any-compiler installer script
# Downloads and installs any-compiler globally on Linux/macOS
#
# Usage: curl -sSL https://raw.githubusercontent.com/ashraf7hossain/any-compiler/main/scripts/install.sh | bash
# Or:    wget -qO- https://raw.githubusercontent.com/ashraf7hossain/any-compiler/main/scripts/install.sh | bash

set -euo pipefail

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
REPO="${REPO:-ashraf7hossain/any-compiler}"
INSTALL_PATH="${INSTALL_PATH:-/usr/local/bin}"
VERSION="${VERSION:-latest}"
ALLOW_SOURCE_FALLBACK="${ALLOW_SOURCE_FALLBACK:-0}"

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
echo "Source fallback enabled: $ALLOW_SOURCE_FALLBACK"

# Determine release tag and release metadata
RELEASE_JSON=""
if [ "$VERSION" = "latest" ]; then
  RELEASE_JSON=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/latest" || true)
  TAG=$(printf "%s" "$RELEASE_JSON" | grep -oE '"tag_name"[[:space:]]*:[[:space:]]*"[^"]+"' | head -1 | cut -d'"' -f4)

  # Fallback: if no latest release exists, try newest git tag.
  if [ -z "$TAG" ]; then
    TAG=$(curl -fsSL "https://api.github.com/repos/$REPO/tags" | grep -oE '"name"[[:space:]]*:[[:space:]]*"[^"]+"' | head -1 | cut -d'"' -f4 || true)
    if [ -n "$TAG" ]; then
      RELEASE_JSON=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/tags/$TAG" || true)
    fi
  fi

  if [ -z "$TAG" ]; then
    echo -e "${RED}✗ Could not determine a version tag${NC}"
    echo "Make sure at least one Git tag/release exists, or provide VERSION explicitly."
    echo "Example: VERSION=1.0.0 curl -sSL https://raw.githubusercontent.com/$REPO/main/scripts/install.sh | bash"
    exit 1
  fi
else
  if [[ "$VERSION" == v* ]]; then
    TAG="$VERSION"
  else
    TAG="v$VERSION"
  fi
  RELEASE_JSON=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/tags/$TAG" || true)
fi

# Prefer exact release asset URL from metadata; fallback to conventional URL.
DOWNLOAD_URL=$(printf "%s" "$RELEASE_JSON" | grep -oE '"browser_download_url"[[:space:]]*:[[:space:]]*"[^"]+"' | cut -d'"' -f4 | grep -E "/any-compiler-$OS-$ARCH$" | head -1 || true)
if [ -z "$DOWNLOAD_URL" ]; then
  DOWNLOAD_URL="https://github.com/$REPO/releases/download/$TAG/any-compiler-$OS-$ARCH"
fi

TARBALL_URL=$(printf "%s" "$RELEASE_JSON" | grep -oE '"tarball_url"[[:space:]]*:[[:space:]]*"[^"]+"' | head -1 | cut -d'"' -f4)

echo "Downloading from: $DOWNLOAD_URL"

# Download binary
if ! curl -fsSL "$DOWNLOAD_URL" -o /tmp/any-compiler; then
  echo -e "${YELLOW}No compatible binary asset found for tag $TAG and target $OS-$ARCH.${NC}"
  if [ "$ALLOW_SOURCE_FALLBACK" != "1" ]; then
    echo -e "${RED}✗ Binary-only install failed.${NC}"
    echo "Upload asset: any-compiler-$OS-$ARCH to release $TAG, or set ALLOW_SOURCE_FALLBACK=1."
    echo "Versioned install example:"
    echo "  curl -fsSL https://raw.githubusercontent.com/$REPO/main/scripts/install.sh | VERSION=1.0.0 REPO=$REPO bash"
    echo "Source fallback example:"
    echo "  curl -fsSL https://raw.githubusercontent.com/$REPO/main/scripts/install.sh | VERSION=1.0.0 ALLOW_SOURCE_FALLBACK=1 REPO=$REPO bash"
    exit 1
  fi

  echo -e "${YELLOW}ALLOW_SOURCE_FALLBACK=1 set, trying source build fallback...${NC}"
  if [ -z "$TARBALL_URL" ]; then
    echo -e "${RED}✗ Failed to download binary and no source tarball URL was found${NC}"
    echo "Please upload release assets or check: https://github.com/$REPO/releases"
    exit 1
  fi

  if ! command -v g++ >/dev/null 2>&1; then
    echo -e "${YELLOW}g++ not found, attempting to install build tools...${NC}"

    SUDO_CMD=""
    if [ "$(id -u)" -ne 0 ]; then
      if command -v sudo >/dev/null 2>&1; then
        SUDO_CMD="sudo"
      else
        echo -e "${RED}✗ Need root or sudo privileges to install g++${NC}"
        exit 1
      fi
    fi

    if command -v apt-get >/dev/null 2>&1; then
      $SUDO_CMD apt-get update && $SUDO_CMD apt-get install -y g++
    elif command -v apk >/dev/null 2>&1; then
      $SUDO_CMD apk add --no-cache g++
    elif command -v dnf >/dev/null 2>&1; then
      $SUDO_CMD dnf install -y gcc-c++
    elif command -v yum >/dev/null 2>&1; then
      $SUDO_CMD yum install -y gcc-c++
    elif command -v pacman >/dev/null 2>&1; then
      $SUDO_CMD pacman -Sy --noconfirm gcc
    else
      echo -e "${RED}✗ Could not auto-install g++; unsupported package manager${NC}"
      echo "Please install g++ manually and rerun this installer."
      exit 1
    fi

    if ! command -v g++ >/dev/null 2>&1; then
      echo -e "${RED}✗ g++ installation attempt failed${NC}"
      exit 1
    fi
  fi

  BUILD_DIR=$(mktemp -d)
  if [ -z "$BUILD_DIR" ] || [ ! -d "$BUILD_DIR" ]; then
    echo -e "${RED}✗ Could not create temporary build directory${NC}"
    exit 1
  fi

  trap 'rm -rf "$BUILD_DIR"' EXIT

  echo "Downloading source tarball: $TARBALL_URL"
  curl -fsSL "$TARBALL_URL" -o "$BUILD_DIR/source.tar.gz"
  tar -xzf "$BUILD_DIR/source.tar.gz" -C "$BUILD_DIR"

  SRC_ROOT=$(find "$BUILD_DIR" -mindepth 1 -maxdepth 1 -type d | head -1)
  if [ -z "$SRC_ROOT" ] || [ ! -f "$SRC_ROOT/src/main.cpp" ]; then
    echo -e "${RED}✗ Source layout not recognized in tarball${NC}"
    exit 1
  fi

  echo "Building from source..."
  BUILD_VERSION="${TAG#v}"
  if ! g++ -O2 -std=c++11 -Wall -Wextra -DAPP_VERSION=\"${BUILD_VERSION}\" "$SRC_ROOT/src/main.cpp" -o /tmp/any-compiler; then
    echo -e "${RED}✗ Source fallback build failed${NC}"
    exit 1
  fi
fi

# Make executable
chmod +x /tmp/any-compiler

# Verify checksum if available
CHECKSUM_URL=$(printf "%s" "$RELEASE_JSON" | grep -oE '"browser_download_url"[[:space:]]*:[[:space:]]*"[^"]+"' | cut -d'"' -f4 | grep -E "/any-compiler-$OS-$ARCH\.sha256$" | head -1 || true)
if [ -z "$CHECKSUM_URL" ]; then
  CHECKSUM_URL="$DOWNLOAD_URL.sha256"
fi

if curl -fsSL "$CHECKSUM_URL" -o /tmp/any-compiler.sha256 2>/dev/null; then
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
