package com.example.minigpt

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import java.util.Locale
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity() {
    private lateinit var recyclerView: RecyclerView
    private lateinit var messageInput: EditText
    private lateinit var sendButton: Button
    private lateinit var adapter: ChatAdapter
    private lateinit var localAgent: LocalIntentAgent
    private val executor = Executors.newSingleThreadExecutor()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        recyclerView = findViewById(R.id.chatRecycler)
        messageInput = findViewById(R.id.messageInput)
        sendButton = findViewById(R.id.sendButton)
        localAgent = LocalIntentAgent(this)

        adapter = ChatAdapter(mutableListOf())
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = adapter
        appendBotMessage("Ciao! Sono MiniGPT, alimentato dall'agente locale di A-0. I messaggi restano sul dispositivo.")

        sendButton.setOnClickListener {
            val input = messageInput.text.toString().trim()
            if (input.isEmpty()) return@setOnClickListener
            appendUserMessage(input)
            messageInput.text?.clear()
            sendButton.isEnabled = false
            executor.execute {
                val reply = localAgent.reply(input)
                runOnUiThread {
                    val confidence = String.format(Locale.ROOT, "%.0f%%", reply.confidence * 100)
                    appendBotMessage("${reply.text}\n\nIntent: ${reply.intent} · confidenza: $confidence")
                    sendButton.isEnabled = true
                }
            }
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

    override fun onDestroy() {
        executor.shutdownNow()
        super.onDestroy()
    }
}
