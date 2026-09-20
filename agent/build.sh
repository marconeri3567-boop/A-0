#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

printf '%s\n' '==================================' ' LocalIntentAI Build Script' '=================================='
printf '[INFO] Project root: %s\n' "${PROJECT_ROOT}"
mkdir -p "${BUILD_DIR}" "${PROJECT_ROOT}/bin" "${PROJECT_ROOT}/models" "${PROJECT_ROOT}/data"

cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
JOBS=2
if command -v nproc >/dev/null 2>&1; then JOBS="$(nproc)"; fi
cmake --build "${BUILD_DIR}" --parallel "${JOBS}"

EXECUTABLE="${PROJECT_ROOT}/bin/local_intent_ai"
if [[ ! -x "${EXECUTABLE}" ]]; then
    echo '[ERROR] Executable not found or not executable.' >&2
    exit 1
fi
printf '[SUCCESS] Executable generated: %s\n' "${EXECUTABLE}"
