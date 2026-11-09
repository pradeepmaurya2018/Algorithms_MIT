import faiss
import numpy as np
from sentence_transformers import SentenceTransformer

model = SentenceTransformer("all-MiniLM-L6-v2")

index = faiss.IndexFlatL2(384)
memory_texts = []

def store(text):
    vec = model.encode([text])
    index.add(vec)
    memory_texts.append(text)

def recall(query, k=3):
    q = model.encode([query])
    D, I = index.search(q, k)
    return [memory_texts[i] for i in I[0]]
