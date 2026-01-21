import requests
import time

URL = "http://localhost:8080/shorten"
payload = {"longUrl": "https://example.com"}

start = time.time()
r = requests.post(URL, json=payload)
end = time.time()

print("Status:", r.status_code)
print("Latency (ms):", (end - start) * 1000)
