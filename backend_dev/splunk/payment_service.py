from flask import Flask, request
import logging, time, random

app = Flask(__name__)

logging.basicConfig(
    filename="payment.log",
    level=logging.INFO,
    format="time=%(asctime)s level=%(levelname)s service=payment "
           "requestId=%(message)s"
)

def fake_db_call():
    delay = random.choice([50, 80, 120, 300, 800])
    time.sleep(delay / 1000)
    return delay

@app.route("/pay")
def pay():
    req_id = random.randint(100000, 999999)
    user = request.args.get("user", "na")

    start = time.time()
    db_latency = fake_db_call()
    total_latency = int((time.time() - start) * 1000)

    if db_latency > 400:
        logging.error(
            f"{req_id} status=FAIL user={user} "
            f"db_latency={db_latency} total_latency={total_latency}"
        )
        return {"status": "FAILED"}, 500

    logging.info(
        f"{req_id} status=SUCCESS user={user} "
        f"db_latency={db_latency} total_latency={total_latency}"
    )
    return {"status": "SUCCESS"}

app.run(port=5000)
