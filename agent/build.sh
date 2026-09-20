#!/data/data/com.termux/files/usr/bin/bash

set -e

echo "=================================="
echo " LocalIntentAI Build Script"
echo "=================================="

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

BUILD_DIR="${PROJECT_ROOT}/build"
BIN_DIR="${PROJECT_ROOT}/bin"
MODELS_DIR="${PROJECT_ROOT}/models"
DATA_DIR="${PROJECT_ROOT}/data"

echo "[INFO] Project root:"
echo "${PROJECT_ROOT}"

mkdir -p "${BUILD_DIR}"
mkdir -p "${BIN_DIR}"
mkdir -p "${MODELS_DIR}"
mkdir -p "${DATA_DIR}"

cd "${BUILD_DIR}"

echo
echo "[INFO] Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo
echo "[INFO] Building project..."

if command -v nproc >/dev/null 2>&1
then
    JOBS=$(nproc)
else
    JOBS=2
fi

cmake --build . -- -j"${JOBS}"

echo
echo "[INFO] Build completed."

EXECUTABLE="${PROJECT_ROOT}/bin/local_intent_ai"

if [ -f "${EXECUTABLE}" ]
then
    echo
    echo "[SUCCESS]"
    echo "Executable generated:"
    echo "${EXECUTABLE}"
else
    echo
    echo "[ERROR] Executable not found."
    exit 1
fi

echo
echo "=================================="
echo " Build Finished"
echo "=================================="
