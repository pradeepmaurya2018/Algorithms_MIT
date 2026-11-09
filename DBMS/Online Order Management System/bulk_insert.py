from psycopg2.extras import execute_values
from db import get_connection

conn = get_connection()
cur = conn.cursor()

BATCH_SIZE = 10_000
TOTAL = 10_000_000

for start in range(1, TOTAL + 1, BATCH_SIZE):
    rows = [
        (i, i % 100000 + 1, 1000.00)
        for i in range(start, min(start + BATCH_SIZE, TOTAL + 1))
    ]

    execute_values(
        cur,
        """
        INSERT INTO orders_big (order_id, user_id, amount, order_date)
        VALUES %s
        """,
        rows,
        template="(%s,%s,%s,now())"
    )

    conn.commit()
    print(f"Inserted up to {i}")

cur.close()
conn.close()
