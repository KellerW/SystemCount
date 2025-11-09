#!/usr/bin/env bash
set -e

BUILD_DIR=build

case "$1" in
    clean)
        echo "🧹 Limpando build..."
        rm -rf "$BUILD_DIR"
        ;;
    Debug|Release|"")
        BUILD_TYPE=${1:-Debug}
        echo "🔧 Build type: $BUILD_TYPE"
        cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
        cmake --build "$BUILD_DIR" -j"$(nproc)"
        ;;
    *)
        echo "Uso: ./build.sh [Debug|Release|clean]"
        exit 1
        ;;
esac