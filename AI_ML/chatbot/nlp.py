import re

def extract_name(text):
    if "my name is" in text.lower():
        return text.split("is")[-1].strip()
    return None

def detect_intent(text: str) -> str:
    text = text.lower()

    if re.search(r"\b(hi|hello|hey)\b", text):
        return "greeting"

    if "name" in text:
        return "ask_name"

    if "time" in text:
        return "time"

    if "bye" in text:
        return "goodbye"


    return "unknown"
