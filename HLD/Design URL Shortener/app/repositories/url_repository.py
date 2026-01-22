import sqlite3

from domain.model import UrlMapping


class UrlRepository:
    def __init__(self, db_name="urls.db"):
        self.db_name = db_name
        self._init_db()

    def _init_db(self):
        conn = sqlite3.connect(self.db_name)
        conn.execute("PRAGMA journal_mode=WAL;")
        conn.execute("PRAGMA synchronous=NORMAL;")
        conn.execute("""
            CREATE TABLE IF NOT EXISTS url_mapping (
                short_code TEXT PRIMARY KEY,
                long_url TEXT NOT NULL
            )
        """)
        conn.close()

    def save(self, mapping: UrlMapping):
        conn = sqlite3.connect(self.db_name)
        conn.execute(
            "INSERT INTO url_mapping (short_code, long_url) VALUES (?, ?)",
            (mapping.short_code, mapping.long_url)
        )
        conn.commit()
        conn.close()

    def find(self, short_code: str):
        conn = sqlite3.connect(self.db_name)
        cur = conn.execute(
            "SELECT long_url FROM url_mapping WHERE short_code = ?",
            (short_code,)
        )
        row = cur.fetchone()
        conn.close()
        return row[0] if row else None
