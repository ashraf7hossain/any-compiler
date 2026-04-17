#!/usr/bin/env bash
#
# Build and install any-compiler from local source on Linux/macOS.
#

set -euo pipefail

SOURCE_DIR="${1:-$(pwd)}"
INSTALL_PATH="${2:-/usr/local/bin}"
VERSION="${3:-dev}"
BINARY_NAME="any-compiler"
TMP_BINARY="/tmp/${BINARY_NAME}"

echo "Building any-compiler from source..."
echo "Source: ${SOURCE_DIR}"
echo "Install path: ${INSTALL_PATH}"

if ! command -v g++ >/dev/null 2>&1; then
  echo "Error: g++ is required but was not found in PATH."
  exit 1
fi

if [ ! -f "${SOURCE_DIR}/src/main.cpp" ]; then
  echo "Error: could not find ${SOURCE_DIR}/src/main.cpp"
  exit 1
fi

g++ -O2 -std=c++11 -Wall -Wextra -DAPP_VERSION=\"${VERSION}\" \
  "${SOURCE_DIR}/src/main.cpp" -o "${TMP_BINARY}"

chmod +x "${TMP_BINARY}"

if [ -w "${INSTALL_PATH}" ]; then
  mkdir -p "${INSTALL_PATH}"
  mv "${TMP_BINARY}" "${INSTALL_PATH}/${BINARY_NAME}"
else
  echo "Installing to ${INSTALL_PATH} requires elevated permissions."
  sudo mkdir -p "${INSTALL_PATH}"
  sudo mv "${TMP_BINARY}" "${INSTALL_PATH}/${BINARY_NAME}"
fi

echo "Installed: ${INSTALL_PATH}/${BINARY_NAME}"
if command -v "${BINARY_NAME}" >/dev/null 2>&1; then
  "${BINARY_NAME}" --version || true
else
  echo "Note: ${BINARY_NAME} is not on PATH in this shell yet."
  echo "Add ${INSTALL_PATH} to PATH if needed."
fi
