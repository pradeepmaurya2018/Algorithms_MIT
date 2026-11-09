from db import get_connection

conn = get_connection()
cur = conn.cursor()

cur.execute("""
INSERT INTO users VALUES
(1, 'Alice', 'alice@mail.com', now()),
(2, 'Bob', 'bob@mail.com', now())
ON CONFLICT DO NOTHING;
""")

cur.execute("""
INSERT INTO products VALUES
(101, 'Laptop', 60000, now()),
(102, 'Mouse', 500, now())
ON CONFLICT DO NOTHING;
""")

cur.execute("""
INSERT INTO inventory VALUES
(101, 10),
(102, 50)
ON CONFLICT DO NOTHING;
""")

conn.commit()
cur.close()
conn.close()
