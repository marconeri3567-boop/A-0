package com.example.minigpt

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.core.content.ContextCompat
import androidx.recyclerview.widget.RecyclerView

data class ChatMessage(
    val text: String,
    val isUser: Boolean
)

class ChatAdapter(private val messages: MutableList<ChatMessage>) :
    RecyclerView.Adapter<ChatAdapter.ChatViewHolder>() {

    inner class ChatViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private val messageText: TextView = itemView.findViewById(R.id.messageText)

        fun bind(message: ChatMessage) {
            messageText.text = message.text

            val backgroundColor = if (message.isUser) {
                ContextCompat.getColor(itemView.context, R.color.user_bubble)
            } else {
                ContextCompat.getColor(itemView.context, R.color.bot_bubble)
            }

            val paddingEnd = if (message.isUser) 12 else 0
            val paddingStart = if (message.isUser) 0 else 12

            itemView.setBackgroundColor(backgroundColor)
            itemView.setPadding(paddingStart, 12, paddingEnd, 12)

            val layoutParams = itemView.layoutParams as RecyclerView.LayoutParams
            layoutParams.marginStart = if (message.isUser) 48 else 0
            layoutParams.marginEnd = if (message.isUser) 0 else 48
            itemView.layoutParams = layoutParams
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ChatViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_chat_message, parent, false)
        return ChatViewHolder(view)
    }

    override fun onBindViewHolder(holder: ChatViewHolder, position: Int) {
        holder.bind(messages[position])
    }

    override fun getItemCount(): Int = messages.size

    fun append(message: ChatMessage) {
        messages.add(message)
        notifyItemInserted(messages.size - 1)
    }
}
