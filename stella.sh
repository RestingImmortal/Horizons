#!/bin/env bash

usage() {
    echo "Usage: $0 {build|clean|release|windows}"
    echo "build --- Builds the project."
    echo "clean --- Removes build directory, leaving executable."
    echo "release - Builds the project in release mode."
    echo "test ---- Runs test suite."
    echo "windows - Cross-compiles the project for Windows in release mode."
    exit 1
}

build_debug() {
    local build_type="Debug"
    configure_and_build "$build_type"
}

build_release() {
    local build_type="Release"
    configure_and_build "$build_type"
}

build_windows() {
    local build_type="Release"
    local target="x86_64-windows-gnu"

    # Add the `scripts/` directory to the PATH dynamically
    export PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/scripts:$PATH"
    export TARGET="$target"

    configure_and_build "$build_type" "$target"
}

configure_and_build() {
    # Arguements
    local build_type="$1"
    local target="$2"

    # Get the directory where Stella is located
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

    # Set build directory
    BUILD_DIR="$SCRIPT_DIR/build"

    # Check if the build directory exists, if not, create it
    if [ ! -d "$BUILD_DIR" ]; then
        echo "Creating build directory..."
        mkdir "$BUILD_DIR"
    fi

    # Navigate to the build directory
    cd "$BUILD_DIR"

    echo "Running CMake..."

    # Base CMake command
    local cmake_cmd="cmake -DCMAKE_BUILD_TYPE=$build_type $SCRIPT_DIR"

    # Add cross-compilation options if target is specified
    if [ -n "$target" ]; then
        cmake_cmd+=" -DTARGET=$target"
        cmake_cmd+=" -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=x86_64"
        cmake_cmd+=" -DCMAKE_C_COMPILER_TARGET=$target -DCMAKE_CXX_COMPILER_TARGET=$target"
    fi

    # Run CMake to configure the project
    eval "$cmake_cmd" || { echo "CMake configuration failed."; exit 1; }

    # Build the project
    echo "Building the project..."
    cmake --build . -j$(nproc) || { echo "Build failed."; exit 1; }

    # Navigate back to the project root
    cd "$SCRIPT_DIR"

    echo "Build complete."
}

clean_build() {
    # If build directory exists, remove it
    if [ -d "./build" ]; then
        echo "Cleaning build directory..."
        rm -rf ./build
        echo "Build directory cleaned."
    fi
}

test_suite() {
    if [ -d "./build" ]; then
        cd ./build
        ctest
        cd ..
    else
        echo "Build the project first."
    fi
}

if [ $# -eq 0 ]; then
    usage
fi

case "$1" in
    build)
        build_debug
        ;;
    clean)
        clean_build
        ;;
    release)
        build_release
        ;;
    test)
        test_suite
        ;;
    windows)
        build_windows
        ;;
    *)
        usage
        ;;
esac
