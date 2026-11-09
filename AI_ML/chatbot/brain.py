import requests
from intent_model import predict_intent
from nlp import detect_intent
import skills
from openai import OpenAI
SKILL_MAP = {
    "greeting": skills.greeting,
    "ask_name": skills.ask_name,
    "time": skills.time_skill,
    "goodbye": skills.goodbye,
    "unknown": skills.unknown
}

from nlp import detect_intent, extract_name

class Brain:
    def respond(self, text, memory):
        name = extract_name(text)
        if name:
            memory.set("user_name", name)
            return f"Nice to meet you, {name}."

        intent = predict_intent(text)
        handler = SKILL_MAP[intent]
        return handler(memory)


    def generate_response(self,user_text, memory_context):
        prompt = f"""
        Conversation memory:
        {memory_context}

        User: {user_text}
        Assistant:
        """

        client = OpenAI()
        response = client.responses.create(
            model="gpt-4.1-mini",
            input=prompt
        )

        return response.output_text

    def generate_response_llama(self, prompt,memory):
        r = requests.post(
            "http://localhost:11434/api/generate",
            json={
                "model": "llama3",
                "prompt": prompt,
                "stream": False
            }
        )
        return r.json()["response"]