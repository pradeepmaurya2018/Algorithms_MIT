import redis

class UrlCache:
    def __init__(self):
        self.client = redis.Redis(
            host="redis",
            port=6379,
            decode_responses=True
        )

    def get(self, short_code: str):
        return self.client.get(short_code)

    def set(self, short_code: str, long_url: str, ttl=3600):
        self.client.setex(short_code, ttl, long_url)
