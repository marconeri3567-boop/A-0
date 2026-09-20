package com.example.minigpt

import android.content.Context
import org.json.JSONArray
import org.json.JSONObject
import java.util.Locale
import kotlin.math.max

/**
 * Agente completamente locale: carica il modello di intenti dagli assets e non
 * effettua richieste di rete. Il formato è compatibile con un export leggero
 * della pipeline presente in agent/.
 */
class LocalIntentAgent(context: Context) {
    data class Reply(val text: String, val intent: String, val confidence: Double)

    private data class IntentModel(
        val name: String,
        val keywords: List<String>,
        val response: String
    )

    private val models: List<IntentModel> = loadModels(context)

    fun reply(prompt: String): Reply {
        val normalized = prompt.lowercase(Locale.ROOT)
        val scored = models.map { model ->
            val matches = model.keywords.count { keyword ->
                normalized.contains(keyword.lowercase(Locale.ROOT))
            }
            model to matches
        }.sortedByDescending { it.second }
        val winner = scored.firstOrNull()
        if (winner == null || winner.second == 0) {
            return Reply(
                "Ho ricevuto: \"$prompt\". Sono un agente locale basato sul modello di intenti incluso nell'app.",
                "unknown",
                0.0
            )
        }
        val confidence = (winner.second.toDouble() / max(1, winner.first.keywords.size)).coerceIn(0.0, 1.0)
        return Reply(winner.first.response.replace("{prompt}", prompt), winner.first.name, confidence)
    }

    private fun loadModels(context: Context): List<IntentModel> {
        val json = context.assets.open("local_model.json").bufferedReader().use { it.readText() }
        val intents = JSONObject(json).getJSONArray("intents")
        return (0 until intents.length()).map { index ->
            val item = intents.getJSONObject(index)
            val keywords = item.getJSONArray("keywords")
            IntentModel(
                name = item.getString("name"),
                keywords = (0 until keywords.length()).map { keywords.getString(it) },
                response = item.getString("response")
            )
        }
    }
}
