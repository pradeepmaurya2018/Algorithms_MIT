from fastapi import APIRouter, HTTPException, Request
from pydantic import BaseModel
from cache.url_cache import UrlCache
from rate_limiter.redis_limiter import RedisRateLimiter

limiter = RedisRateLimiter(limit=10, window_seconds=1)
router = APIRouter()

class ShortenRequest(BaseModel):
    longUrl: str
cache = UrlCache()

def init_routes(service, server_id):

    @router.post("/shorten")
    def shorten(req: ShortenRequest, request: Request):
        client_ip = request.client.host

        if not limiter.allow(client_ip):
            raise HTTPException(
                status_code=429,
                detail="Rate limit exceeded. Try again later."
            )
        mapping = service.shorten(req.longUrl)
        # print("Handled by PID:", server_id)
        cache.set(mapping.short_code, mapping.long_url)
        return {
            "shortUrl": f"http://localhost/{mapping.short_code}",
            "handled_by_pid": server_id
        }

    @router.get("/{short_code}")
    def redirect(short_code: str):


        cached = cache.get(short_code)
        if cached:
            return {"redirect_to": cached}

        long_url = service.resolve(short_code)
        if not long_url:
            raise HTTPException(status_code=404, detail="URL not found")

        cache.set(short_code, long_url)
        return {"redirect_to": long_url}

    return router
