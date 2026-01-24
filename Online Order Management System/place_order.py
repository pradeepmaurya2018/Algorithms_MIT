from db import get_connection

def place_order():
    conn = get_connection()
    cur = conn.cursor()

    try:
        conn.autocommit = False  # START TRANSACTION

        cur.execute(
            "INSERT INTO orders VALUES (%s, %s, %s, now())",
            (1001, 1, 'CREATED')
        )

        cur.execute(
            "INSERT INTO order_items VALUES (%s, %s, %s, %s)",
            (1001, 101, 1, 60000)
        )

        cur.execute(
            "UPDATE inventory SET quantity = quantity - 1 WHERE product_id = %s",
            (101,)
        )

        cur.execute(
            "INSERT INTO payments VALUES (%s, %s, %s, %s, now())",
            (9001, 1001, 'SUCCESS', 60000)
        )

        conn.commit()
        print("Order placed successfully")

    except Exception as e:
        conn.rollback()
        print("Transaction failed:", e)

    finally:
        cur.close()
        conn.close()

place_order()
