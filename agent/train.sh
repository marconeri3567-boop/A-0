#!/data/data/com.termux/files/usr/bin/bash

set -e

echo "=================================="
echo " LocalIntentAI Training Script"
echo "=================================="

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

EXECUTABLE="${PROJECT_ROOT}/bin/local_intent_ai"

DEFAULT_DATASET="${PROJECT_ROOT}/data/intents.json"

DATASET_FILE="${1:-$DEFAULT_DATASET}"

if [ ! -f "${EXECUTABLE}" ]
then
    echo
    echo "[ERROR] Executable not found:"
    echo "${EXECUTABLE}"
    echo
    echo "Run:"
    echo "./build.sh"
    exit 1
fi

if [ ! -f "${DATASET_FILE}" ]
then
    echo
    echo "[ERROR] Dataset not found:"
    echo "${DATASET_FILE}"
    exit 1
fi

echo
echo "[INFO] Dataset:"
echo "${DATASET_FILE}"

echo
echo "[INFO] Starting training..."
echo

"${EXECUTABLE}" train "${DATASET_FILE}"

RESULT=$?

echo

if [ ${RESULT} -eq 0 ]
then
    echo "[SUCCESS] Training completed."
    echo

    echo "Generated artifacts:"

    echo "models/model.dat"
    echo "models/labels.json"
    echo "models/vocabulary.json"
else
    echo "[ERROR] Training failed."
fi

echo
echo "=================================="
echo " Training Finished"
echo "=================================="

exit ${RESULT}
