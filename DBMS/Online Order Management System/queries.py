from db import get_connection

conn = get_connection()
cur = conn.cursor()

cur.execute("""
SELECT o.order_id, p.name, oi.quantity
FROM orders o
JOIN order_items oi ON o.order_id = oi.order_id
JOIN products p ON oi.product_id = p.product_id
WHERE o.user_id = %s
""", (1,))

for row in cur.fetchall():
    print(row)

cur.close()
conn.close()
