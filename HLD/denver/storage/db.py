import psycopg2
from psycopg2.extras import execute_batch

class PacketDB:
    def __init__(self):
        psycopg2.extras.register_uuid()
        self.conn = psycopg2.connect(
            dbname="packet_validation",
            user="postgres",
            password="postgres",
            host="localhost"
        )
        self.conn.autocommit = True

    def insert_packets(self, records):
        sql = """
        INSERT INTO packet_metadata
        (ts, src_ip, dst_ip, protocol, src_port, dst_port,
         packet_size, direction, test_name, run_id)
        VALUES (%(ts)s, %(src_ip)s, %(dst_ip)s, %(protocol)s,
                %(src_port)s, %(dst_port)s, %(packet_size)s,
                %(direction)s, %(test_name)s, %(run_id)s)
        """
        with self.conn.cursor() as cur:
            execute_batch(cur, sql, records)
