#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
EXECUTABLE="${PROJECT_ROOT}/bin/local_intent_ai"
DEFAULT_DATASET="${PROJECT_ROOT}/res/dataset/JSON/EnglishToBash.json"
DATASET_FILE="${1:-${DEFAULT_DATASET}}"

if [[ ! -x "${EXECUTABLE}" ]]; then
    echo '[ERROR] Executable not found. Run ./build.sh first.' >&2
    exit 1
fi
if [[ ! -f "${DATASET_FILE}" ]]; then
    printf '[ERROR] Dataset not found: %s\n' "${DATASET_FILE}" >&2
    exit 1
fi

cd "${PROJECT_ROOT}"
"${EXECUTABLE}" train "${DATASET_FILE}"
