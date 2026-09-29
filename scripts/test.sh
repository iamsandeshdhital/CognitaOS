#!/bin/bash
# CognitaOS test script

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$ROOT_DIR/build"

echo "CognitaOS Test Script"
echo "====================="
echo ""

# Build if needed
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Building..."
    "$SCRIPT_DIR/build.sh"
fi

cd "$BUILD_DIR"

# Run tests
echo "Running tests..."
ctest --output-on-failure

echo ""
echo "All tests passed!"
