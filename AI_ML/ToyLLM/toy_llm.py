import torch
import torch.nn as nn
import torch.optim as optim
import os

# -----------------------
# Load dataset
# -----------------------

with open("/home/pradeep/hdl/Algorithms_MIT/AI_ML/ToyLLM/11-0.txt", "r", encoding="utf-8") as f:
    text = f.read().lower()

print("dataset size:", len(text), "characters")

chars = sorted(list(set(text)))
stoi = {ch: i for i, ch in enumerate(chars)}
itos = {i: ch for ch, i in stoi.items()}
vocab_size = len(chars)

def encode(s):
    return torch.tensor([stoi[c] for c in s], dtype=torch.long)

def decode(t):
    return ''.join([itos[i.item()] for i in t])

data = encode(text)

# -----------------------
# Device setup
# -----------------------

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print("Using device:", device)

data = data.to(device)

# -----------------------
# Mini-batch loader
# -----------------------

batch_size = 64
block_size = 64

def get_batch():
    ix = torch.randint(len(data) - block_size, (batch_size,))
    x = torch.stack([data[i:i+block_size] for i in ix])
    y = torch.stack([data[i+1:i+block_size+1] for i in ix])
    return x.to(device), y.to(device)

# -----------------------
# Model
# -----------------------

class TinyLLM(nn.Module):
    def __init__(self):
        super().__init__()
        self.embed = nn.Embedding(vocab_size, 64)
        self.dropout = nn.Dropout(0.2)
        self.rnn = nn.GRU(64, 128, batch_first=True)
        self.fc = nn.Linear(128, vocab_size)

    def forward(self, x, h=None):
        x = self.embed(x)
        x = self.dropout(x)
        out, h = self.rnn(x, h)
        out = self.fc(out)
        return out, h

model = TinyLLM().to(device)

# -----------------------
# Load or train
# -----------------------

loss_fn = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=0.003)

if os.path.exists("toy_llm.pt"):
    model.load_state_dict(torch.load("toy_llm.pt", map_location=device))
    print("Loaded saved model!")
else:
    print("Training from scratch...")

    for epoch in range(3000):
        Xb, Yb = get_batch()

        logits, _ = model(Xb)
        loss = loss_fn(logits.view(-1, vocab_size), Yb.view(-1))

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        if epoch % 200 == 0:
            print("epoch", epoch, "loss", loss.item())

    torch.save(model.state_dict(), "toy_llm.pt")
    print("Model saved!")

# -----------------------
# Generation
# -----------------------

def generate(start="under ", length=10, temperature=1.2):
    model.eval()

    input = encode(start).unsqueeze(0).to(device)
    h = None
    result = start

    for _ in range(length):
        logits, h = model(input[:, -1:], h)

        probs = torch.softmax(logits[0, -1] / temperature, dim=0)
        idx = torch.multinomial(probs, 1)

        char = decode(idx)
        result += char

        input = torch.cat([input, idx.unsqueeze(0)], dim=1)

    return result

print("\nGenerated text:\n")
print(generate())
