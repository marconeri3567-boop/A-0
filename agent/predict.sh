#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
EXECUTABLE="${PROJECT_ROOT}/bin/local_intent_ai"

if [[ ! -x "${EXECUTABLE}" ]]; then
    echo '[ERROR] Executable not found. Run ./build.sh first.' >&2
    exit 1
fi
if [[ $# -eq 0 ]]; then
    echo 'Usage: ./predict.sh "text"' >&2
    exit 1
fi

cd "${PROJECT_ROOT}"
"${EXECUTABLE}" predict "$*"
