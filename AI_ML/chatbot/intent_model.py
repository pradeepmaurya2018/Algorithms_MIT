from sklearn.feature_extraction.text import CountVectorizer
from sklearn.linear_model import LogisticRegression

training_data = [
    ("hello hi hey", "greeting"),
    ("what is my name", "ask_name"),
    ("tell me the time", "time"),
    ("bye goodbye", "goodbye"),
]

texts = [t for t, label in training_data]
labels = [label for t, label in training_data]

vectorizer = CountVectorizer()
X = vectorizer.fit_transform(texts)

model = LogisticRegression()
model.fit(X, labels)

def predict_intent(text):
    vec = vectorizer.transform([text])
    return model.predict(vec)[0]
