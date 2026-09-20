# A-0 — MiniGPT Android con agente locale

A-0 è un'app Android di chat offline. La UI invia ogni messaggio a un agente di intenti locale, caricato dall'APK, invece di usare una risposta mock nell'Activity o un'API cloud.

## Flusso aggiornato

```text
messaggio utente
      ↓
MainActivity → LocalIntentAgent
      ↓             ↓
 RecyclerView   assets/local_model.json
      ↓
risposta + intento + confidenza
```

`LocalIntentAgent` carica il modello da `app/src/main/assets/local_model.json`, assegna un intento tramite scoring delle parole chiave e produce la risposta sul dispositivo. Non sono richiesti account, chiavi API o connessione Internet. Il dataset e il trainer C++ originali restano in `agent/` come pipeline desktop e riferimento per futuri export del modello.

## Build dell'APK

Requisiti: JDK 17, Android SDK 34, Gradle (oppure `gradlew`) e un ambiente Android configurato.

```bash
# APK debug installabile
bash ./build-apk.sh

# APK release non firmato (per distribuzione va firmato con una chiave Android)
bash ./build-apk.sh --release
```

Gli artefatti vengono copiati in `dist/`. Per installare il debug APK su un dispositivo con ADB:

```bash
adb install -r dist/A-0-debug.apk
```

Se usi Android Studio puoi aprire il progetto, sincronizzare Gradle ed eseguire `app` su un emulatore o dispositivo API 24+.

## Struttura

- `app/`: modulo Android Kotlin.
- `app/src/main/java/com/example/minigpt/LocalIntentAgent.kt`: runtime locale del modello.
- `app/src/main/assets/local_model.json`: modello di intenti incluso nell'APK.
- `app/src/main/java/com/example/minigpt/MainActivity.kt`: flusso chat asincrono verso l'agente.
- `agent/`: agente C++ desktop, dataset, training e predizione storici.
- `build-apk.sh`: build riproducibile e raccolta dell'APK in `dist/`.

## Modello locale e privacy

Il modello incluso è un classificatore leggero di intenti, non un LLM generativo. Il testo viene elaborato in memoria sul dispositivo e non viene trasmesso. Per estendere il comportamento, aggiorna gli intenti e le keyword in `local_model.json`; per un modello neurale più grande occorre aggiungere un export mobile (ad esempio TFLite/ONNX) e il relativo runtime.

## Sviluppo del modello desktop

La pipeline C++ può essere compilata separatamente:

```bash
cd agent
bash ./build.sh
bash ./train.sh
bash ./predict.sh "open the browser"
```

L'APK non esegue il binario desktop: usa l'export leggero in `assets`, così la build Android rimane portabile e completamente offline.

## Licenza e stato

Prototipo open source. La release è non firmata e deve essere firmata prima della pubblicazione su store Android.
