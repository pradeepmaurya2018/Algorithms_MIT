from brain import Brain
from memory import Memory

brain = Brain()
memory = Memory()

print("Chatbot ready. Type 'bye' to exit.")

while True:
    user_input = input("You: ")

    if user_input.lower() == "bye":
        print("Bot:", brain.generate_response_llama(user_input, memory))
        break

    response = brain.generate_response_llama(user_input, memory)
    print("Bot:", response)
