from flask import Flask, request
import logging
import random
import time

app = Flask(__name__)

logging.basicConfig(
    filename="app.log",
    level=logging.INFO,
    format="time=%(asctime)s level=%(levelname)s service=payment api=%(message)s"
)

@app.route("/pay")
def pay():
    user = request.args.get("user", "unknown")
    start = time.time()

    if random.random() < 0.3:
        latency = int((time.time() - start) * 1000)
        logging.error(f"failure user={user} latency={latency}")
        return {"status": "failed"}, 500

    latency = int((time.time() - start) * 1000)
    logging.info(f"success user={user} latency={latency}")
    return {"status": "ok"}

app.run(port=5000)
