import psycopg2
from domain.model import UrlMapping

class UrlRepositoryPostgres:
    def __init__(self):
        self.conn = psycopg2.connect(
            dbname="url_shortener",
            user="url_user",
            password="url_pass",
            host="postgres",
            port=5432
        )
        self._init_db()

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
