#!/data/data/com.termux/files/usr/bin/bash

set -e

echo "=================================="
echo " LocalIntentAI Prediction Script"
echo "=================================="

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

EXECUTABLE="${PROJECT_ROOT}/bin/local_intent_ai"

MODEL_FILE="${PROJECT_ROOT}/models/model.dat"
LABEL_FILE="${PROJECT_ROOT}/models/labels.json"
VOCAB_FILE="${PROJECT_ROOT}/models/vocabulary.json"

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

if [ ! -f "${MODEL_FILE}" ]
then
    echo
    echo "[ERROR] Missing file:"
    echo "${MODEL_FILE}"
    echo
    echo "Train the model first:"
    echo "./train.sh"
    exit 1
fi

if [ ! -f "${LABEL_FILE}" ]
then
    echo
    echo "[ERROR] Missing file:"
    echo "${LABEL_FILE}"
    exit 1
fi

if [ ! -f "${VOCAB_FILE}" ]
then
    echo
    echo "[ERROR] Missing file:"
    echo "${VOCAB_FILE}"
    exit 1
fi

if [ $# -lt 1 ]
then
    echo
    echo "Usage:"
    echo "./predict.sh \"apri firefox\""
    exit 1
fi

INPUT_TEXT="$*"

echo
echo "[INPUT]"
echo "${INPUT_TEXT}"

echo
echo "[RESULT]"
echo

"${EXECUTABLE}" predict "${INPUT_TEXT}"

echo
echo "=================================="
echo " Prediction Finished"
echo "=================================="
