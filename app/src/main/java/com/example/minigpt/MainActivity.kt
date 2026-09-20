package com.example.minigpt

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

class MainActivity : AppCompatActivity() {

    private lateinit var recyclerView: RecyclerView
    private lateinit var messageInput: EditText
    private lateinit var sendButton: Button
    private lateinit var adapter: ChatAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        recyclerView = findViewById(R.id.chatRecycler)
        messageInput = findViewById(R.id.messageInput)
        sendButton = findViewById(R.id.sendButton)

        adapter = ChatAdapter(mutableListOf())
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = adapter

        appendBotMessage("Ciao! Sono MiniGPT. Chiedimi qualcosa e ti risponderò in modo rapido e naturale.")

        sendButton.setOnClickListener {
            val input = messageInput.text.toString().trim()
            if (input.isEmpty()) return@setOnClickListener

            appendUserMessage(input)
            messageInput.text?.clear()

            val reply = generateReply(input)
            appendBotMessage(reply)
        }
    }

    private fun appendUserMessage(text: String) {
        adapter.append(ChatMessage(text, true))
        recyclerView.scrollToPosition(adapter.itemCount - 1)
    }

    private fun appendBotMessage(text: String) {
        adapter.append(ChatMessage(text, false))
        recyclerView.scrollToPosition(adapter.itemCount - 1)
    }

    private fun generateReply(prompt: String): String {
        val normalized = prompt.lowercase()

        return when {
            normalized.contains("ciao") || normalized.contains("hello") ->
                "Ciao! Sono qui per aiutarti. Che cosa vuoi fare oggi?"
            normalized.contains("saluto") || normalized.contains("come stai") ->
                "Sto bene, grazie. Sono una demo minimale di assistente conversazionale per Android."
            normalized.contains("aiuto") || normalized.contains("help") ->
                "Posso aiutarti a scrivere un messaggio, a spiegare un concetto o a generare un prompt. Prova: 'spiega il tuo design'."
            normalized.contains("design") || normalized.contains("architettura") ->
                "L'app usa una singola Activity con RecyclerView, messaggi utente e risposta del bot in memoria. È semplice, leggibile e facile da estendere."
            normalized.contains("prezzo") || normalized.contains("costo") ->
                "Questa è una demo open-source e locale, senza chiamate a API esterne. Puoi collegarla facilmente a OpenAI o a un backend personale."
            normalized.contains("apri") || normalized.contains("lancia") ->
                "In una versione reale, potrei aprire una schermata, invocare un tool o contattare un servizio esterno."
            else ->
                "Ho capito: \"$prompt\". In questo prototipo rispondo con logica locale e puoi sostituire questa parte con un vero backend AI."
        }
    }
}
