import time
import psycopg2
from domain.model import UrlMapping

class UrlRepositoryPostgres:
    def __init__(self):
        self.conn = self._connect_with_retry()
        self._init_db()

    def _connect_with_retry(self):
        for attempt in range(10):
            try:
                print("Connecting to Postgres...")
                return psycopg2.connect(
                    dbname="url_shortener",
                    user="url_user",
                    password="url_pass",
                    host="postgres",
                    port=5432
                )
            except psycopg2.OperationalError as e:
                print(f"Postgres not ready, retrying ({attempt + 1}/10)...")
                time.sleep(2)
        raise RuntimeError("Could not connect to Postgres after retries")

    def _init_db(self):
        with self.conn.cursor() as cur:
            cur.execute("""
                CREATE TABLE IF NOT EXISTS url_mapping (
                    short_code VARCHAR(20) PRIMARY KEY,
                    long_url TEXT NOT NULL
                )
            """)
            self.conn.commit()

    def save(self, mapping: UrlMapping):
        with self.conn.cursor() as cur:
            cur.execute(
                "INSERT INTO url_mapping (short_code, long_url) VALUES (%s, %s)",
                (mapping.short_code, mapping.long_url)
            )
            self.conn.commit()

    def find(self, short_code: str):
        with self.conn.cursor() as cur:
            cur.execute(
                "SELECT long_url FROM url_mapping WHERE short_code = %s",
                (short_code,)
            )
            row = cur.fetchone()
            return row[0] if row else None
