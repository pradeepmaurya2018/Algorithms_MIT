from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from cache.url_cache import UrlCache

router = APIRouter()

class ShortenRequest(BaseModel):
    longUrl: str
cache = UrlCache()

def init_routes(service, server_id):

    @router.post("/shorten")
    def shorten(req: ShortenRequest):
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
