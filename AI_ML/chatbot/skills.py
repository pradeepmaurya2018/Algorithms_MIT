from datetime import datetime

def greeting(memory):
    return "Hello! How can I help you?"

def ask_name(memory):
    name = memory.get("user_name")
    if name:
        return f"Your name is {name}."
    return "I don't know your name yet."

def time_skill(memory):
    return f"The time is {datetime.now().strftime('%H:%M:%S')}"

def goodbye(memory):
    return "Goodbye!"

def unknown(memory):
    return "I don't understand that yet."
