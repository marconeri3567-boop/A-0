#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
APK_DIR="${ROOT_DIR}/dist"
GRADLE_CMD=""

if [[ -x "${ROOT_DIR}/gradlew" ]]; then
  GRADLE_CMD="${ROOT_DIR}/gradlew"
elif command -v gradle >/dev/null 2>&1; then
  GRADLE_CMD="$(command -v gradle)"
else
  echo "[ERROR] Gradle non trovato. Installa Android Studio/Gradle oppure aggiungi gradlew al repository." >&2
  exit 1
fi

if ! command -v java >/dev/null 2>&1; then
  echo "[ERROR] JDK 17 richiesto ma Java non è disponibile nel PATH." >&2
  exit 1
fi

JAVA_MAJOR="$(java -version 2>&1 | sed -nE 's/.*version "([0-9]+).*/\1/p' | head -n1)"
if [[ "${JAVA_MAJOR}" != "17" ]]; then
  echo "[WARN] Rilevato Java ${JAVA_MAJOR:-sconosciuto}; il progetto è verificato con JDK 17." >&2
fi

TASK="assembleDebug"
if [[ "${1:-}" == "--release" ]]; then
  TASK="assembleRelease"
  shift
fi

if [[ $# -gt 0 ]]; then
  echo "Uso: $0 [--release]" >&2
  exit 2
fi

cd "${ROOT_DIR}"
echo "[INFO] Compilazione Android (${TASK}) con agente locale incluso..."
"${GRADLE_CMD}" --no-daemon "${TASK}"

mkdir -p "${APK_DIR}"
APK="$(find "${ROOT_DIR}/app/build/outputs/apk" -type f -name '*.apk' -print -quit)"
if [[ -z "${APK}" ]]; then
  echo "[ERROR] APK non generato." >&2
  exit 1
fi

OUTPUT="${APK_DIR}/A-0-${TASK#assemble}.apk"
cp "${APK}" "${OUTPUT}"
printf '[SUCCESS] APK disponibile: %s\n' "${OUTPUT}"
printf '[INFO] Installazione opzionale: adb install -r "%s"\n' "${OUTPUT}"
