#!/bin/bash
# CognitaOS build script

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$ROOT_DIR/build"

echo "CognitaOS Build Script"
echo "======================"
echo ""

# Parse arguments
BUILD_TYPE="Release"
CLEAN=0

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            CLEAN=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Clean if requested
if [ $CLEAN -eq 1 ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "Configuring (Build type: $BUILD_TYPE)..."
cmake .. -GNinja -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# Build
echo "Building..."
ninja

echo ""
echo "Build complete!"
echo "  Library: $BUILD_DIR/lib/libcognita.so"
echo "  Tests:   $BUILD_DIR/bin/test_*"
echo "  Examples: $BUILD_DIR/bin/hello_intent, agent_orchestration, ..."
echo ""
echo "Run tests with: cd $BUILD_DIR && ctest --output-on-failure"
