#!/bin/bash
# Post-install script for any-compiler

# Verify the binary is executable
chmod +x /usr/local/bin/any-compiler

# Verify installation
if /usr/local/bin/any-compiler --version &>/dev/null; then
  echo "any-compiler installed successfully"
else
  echo "Warning: any-compiler may not be properly installed"
  exit 1
fi
