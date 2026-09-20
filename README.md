# LocalIntentAI

LocalIntentAI è un assistente locale offline scritto in C++20. Trasforma una frase in un intent tramite un classificatore neurale Tiny-DNN, conserva il contesto della conversazione e decide quale tool invocare tramite un decision tree.

## Funzionalità

- classificazione intenti con rete neurale e rappresentazione Bag-of-Words;
- training da dataset JSON e salvataggio di modello, label e vocabolario;
- inferenza separata dal layer operativo dei tool;
- decision tree per soglia di confidenza, contesto precedente e routing sicuro;
- persistenza della cronologia in JSON e delle metriche in SQLite;
- controllo dell'ambiente con suggerimenti per installare le dipendenze mancanti;
- dipendenze C++ header-only recuperabili automaticamente da CMake FetchContent.

## Struttura

```text
agent/
├── include/
│   ├── ContextStore.hpp       # stato persistente della conversazione
│   ├── DatasetLoader.hpp      # caricamento dei campioni
│   ├── Vocabulary.hpp         # testo -> vettore BoW
│   ├── NeuralModel.hpp        # rete Tiny-DNN e predizione
│   ├── Trainer.hpp            # pipeline di training
│   ├── Predictor.hpp          # caricamento e inferenza
│   ├── ToolDispatcher.hpp     # registrazione e invocazione tool
│   ├── DecisionTree.hpp       # regole decisionali di routing
│   └── DependencyChecker.hpp  # verifica tool e suggerimenti
├── src/                      # implementazioni C++
├── res/dataset/JSON/         # dataset di esempio
├── models/                   # artefatti runtime generati
└── CMakeLists.txt
```

## Dipendenze

Sono necessari un compilatore C++20, CMake 3.16+, Git e SQLite con gli header di sviluppo. Eigen, nlohmann/json e tiny-dnn vengono cercati localmente e, se assenti, scaricati da CMake durante la configurazione.

Prima della compilazione è possibile verificare l'ambiente:

```bash
./agent/bin/local_intent_ai check-dependencies
```

Il comando non installa pacchetti senza consenso e non usa privilegi elevati. Se trova componenti mancanti mostra il package manager rilevato e il comando suggerito. Esempi:

```bash
# Termux
pkg install clang cmake git make sqlite

# Debian/Ubuntu
sudo apt-get update && sudo apt-get install -y build-essential cmake git libsqlite3-dev

# Fedora
sudo dnf install gcc-c++ cmake git sqlite-devel
```

Quando il binario non è ancora stato compilato, il controllo può essere eseguito dopo aver configurato il progetto o sostituito con il controllo manuale del package manager.

## Build

Dalla root del repository:

```bash
cmake -S agent -B agent/build -DCMAKE_BUILD_TYPE=Release
cmake --build agent/build -j
```

Il binario viene creato in `agent/bin/local_intent_ai`. La prima configurazione può richiedere rete per FetchContent; dopo il download CMake riutilizza la cache.

## Training

Il loader supporta campioni nel formato:

```json
[
  {"input":"open the browser", "intent":"open_browser"},
  {"input":"start the editor", "intent":"open_editor"}
]
```

Avvio:

```bash
agent/bin/local_intent_ai train path/to/dataset.json
```

Il training costruisce il vocabolario, ordina gli intenti, crea i vettori BoW, addestra la rete e salva:

- `models/model.dat` — pesi Tiny-DNN;
- `models/labels.json` — dimensioni e mapping intenti;
- `models/vocabulary.json` — vocabolario;
- `models/local_intent_ai.db` — log e metriche;
- `models/context.json` — contesto persistente.

## Inferenza e flusso decisionale

```bash
agent/bin/local_intent_ai predict "open the browser"
```

Il flusso è:

1. `Predictor` carica vocabolario, label e pesi.
2. Il testo viene convertito in BoW.
3. `NeuralModel` produce intent e confidenza.
4. `ContextStore` carica il contesto precedente e salva la richiesta corrente.
5. `ToolDispatcher` crea una `ToolRequest`.
6. `DecisionTree` valuta la richiesta:
   - confidenza `< 0.60` → `clarify_request`;
   - stesso intent consecutivo → `repeat_action`;
   - altrimenti → intent originale.
7. Il dispatcher invoca esclusivamente l'handler della route scelta.
8. La risposta JSON include predizione, decisione, contesto ed esito del tool.

Il decision tree non esegue comandi di sistema: protegge il routing. I tool reali possono essere aggiunti registrando handler in `registerDefaultTools` o in un componente dedicato, con validazione degli input e senza concatenare testo utente in shell command.

## Verifica e manutenzione

Dopo ogni modifica:

```bash
cmake --build agent/build -j
agent/bin/local_intent_ai check-dependencies
```

La build usa warning rigorosi (`-Wall -Wextra -Wpedantic -Wconversion -Wshadow`). In ambienti offline è necessario avere già disponibili i sorgenti FetchContent o installare le librerie localmente.

## Sicurezza e limiti

Il progetto è un classificatore/routing offline, non un modello linguistico generativo. La confidenza è una stima della rete e non una garanzia: il decision tree riduce le azioni incerte ma non sostituisce autorizzazioni, sandbox o conferma utente per operazioni distruttive. Gli handler di produzione devono applicare una allow-list, validare entity e gestire gli errori.
