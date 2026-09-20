# MiniGPT Android

Questo repository è stato trasformato in una app Android minimale ispirata all'interfaccia di ChatGPT/OpenAI: una chat single-screen, con messaggi utente e risposta del bot, progettata come prototipo facile da far crescere con un backend AI reale.

## Obiettivo

L'idea iniziale del progetto era un assistente locale in C++ basato su classificazione intenti e routing decisionale. Ora il focus è stato spostato su una demo Android dedicata alla chat conversazionale, mantenendo l'esperienza semplice, leggibile e pronta per evoluzione.

## Cosa cambia rispetto al progetto C++

- la vecchia base `agent/` è stata mantenuta come riferimento storico;
- la nuova direzione è un'app Android moderna e minimale;
- l'interfaccia è simile a ChatGPT: elenco messaggi, input testuale e risposta del bot;
- la logica di risposta è locale e mock, ma facilmente sostituibile con OpenAI, Gemini, Azure OpenAI o un backend interno.

## Struttura del progetto

```text
.
├── app/
│   ├── build.gradle.kts
│   ├── proguard-rules.pro
│   └── src/
│       ├── main/
│       │   ├── AndroidManifest.xml
│       │   ├── java/com/example/minigpt/
│       │   │   ├── ChatAdapter.kt
│       │   │   └── MainActivity.kt
│       │   └── res/
│           ├── layout/
│           │   ├── activity_main.xml
│           │   └── item_chat_message.xml
│           ├── values/
│           │   ├── colors.xml
│           │   ├── strings.xml
│           │   └── themes.xml
│           └── xml/
├── agent/                  # vecchio prototipo C++ preservato per riferimento
├── README.md
├── build.gradle.kts
├── gradle.properties
├── settings.gradle.kts
└── .gitignore
```

## Funzionalità della demo

- chat UI minimale a schermo unico;
- prompt dell'utente e risposta del bot;
- layout Material 3 con palette scura simile a ChatGPT;
- logica di risposta locale con regole semplici;
- pronta per essere collegata a un backend AI reale.

## Requisiti

- Android Studio Ladybug o versione successiva;
- JDK 17;
- Android SDK 34;
- dispositivo/emulatore Android con API 24+.

## Esecuzione

1. Apri il repository in Android Studio.
2. Sincronizza il progetto con Gradle.
3. Seleziona un emulatore o un dispositivo fisico.
4. Avvia l'app.

### Build con Gradle (se usato da CLI)

```bash
./gradlew assembleDebug
```

Se il wrapper non è presente, usa Android Studio per generare la build e sincronizzare il progetto.

## Come evolverla

In una seconda fase puoi collegare l'app a un vero servizio AI:

- OpenAI API con `OkHttp` o `Retrofit`;
- Gemini / Azure OpenAI;
- backend personale con streaming risposta;
- persistenza chat locale in Room o DataStore.

## Note

Questa è una demo minimale e leggera, pensata per dimostrare la trasformazione del progetto in un'app Android "simile a ChatGPT" senza dipendenze esterne pesanti. Il comportamento corrente è locale e deterministico, ma l'architettura è pronta ad essere ampliata.
