#!/bin/bash
# CognitaOS code formatting script

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

echo "CognitaOS Code Formatter"
echo "========================"
echo ""

# Check if clang-format is available
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found. Please install it."
    exit 1
fi

# Format all source files
echo "Formatting source files..."
find "$ROOT_DIR/src" "$ROOT_DIR/include" "$ROOT_DIR/tests" "$ROOT_DIR/examples" \
    \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} \;

echo ""
echo "Formatting complete!"
