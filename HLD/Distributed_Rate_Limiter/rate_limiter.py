#
#
# POST /ratelimit/
# GET /ratelimit/status/{client_id}
# PUT /ratelimit/rules

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import time

app = FastAPI()

# -----------------------------
# Data Models
# -----------------------------

class RateLimitRule(BaseModel):
    capacity: int           # max tokens
    refill_rate: float      # tokens per second

class RateLimitRequest(BaseModel):
    client_id: str

# -----------------------------
# In-Memory Stores
# -----------------------------

rules = RateLimitRule(capacity=5, refill_rate=1.0)

buckets = {}
# client_id -> { tokens, last_refill_timestamp }

# -----------------------------
# Helper Functions
# -----------------------------

def refill_bucket(client_id: str):
    now = time.time()

    if client_id not in buckets:
        buckets[client_id] = {
            "tokens": rules.capacity,
            "last_refill": now
        }
        return

    bucket = buckets[client_id]
    elapsed = now - bucket["last_refill"]

    refill_tokens = elapsed * rules.refill_rate
    bucket["tokens"] = min(
        rules.capacity,
        bucket["tokens"] + refill_tokens
    )
    bucket["last_refill"] = now

# -----------------------------
# APIs
# -----------------------------

@app.post("/ratelimit/")
def check_rate_limit(req: RateLimitRequest):
    refill_bucket(req.client_id)

    bucket = buckets[req.client_id]

    if bucket["tokens"] >= 1:
        bucket["tokens"] -= 1
        return {
            "allowed": True,
            "remaining_tokens": int(bucket["tokens"])
        }

    return {
        "allowed": False,
        "retry_after_seconds": 1 / rules.refill_rate
    }


@app.get("/ratelimit/status/{client_id}")
def get_status(client_id: str):
    refill_bucket(client_id)
    bucket = buckets[client_id]

    return {
        "client_id": client_id,
        "tokens": bucket["tokens"],
        "capacity": rules.capacity,
        "refill_rate": rules.refill_rate
    }


@app.put("/ratelimit/rules")
def update_rules(new_rules: RateLimitRule):
    global rules
    rules = new_rules
    buckets.clear()  # reset all buckets on rule change

    return {
        "message": "Rate limit rules updated",
        "rules": rules
    }

