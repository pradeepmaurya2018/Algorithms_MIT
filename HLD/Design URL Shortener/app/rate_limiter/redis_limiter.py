import time
import redis

class RedisRateLimiter:
    def __init__(self, limit: int, window_seconds: int):
        self.limit = limit
        self.window = window_seconds
        self.redis = redis.Redis(
            host="redis",
            port=6379,
            decode_responses=True
        )

    def allow(self, key: str) -> bool:
        """
        Returns True if request is allowed, False otherwise
        """
        now = int(time.time())
        window_key = f"rate:{key}:{now // self.window}"

        count = self.redis.incr(window_key)
        if count == 1:
            self.redis.expire(window_key, self.window)

        return count <= self.limit
