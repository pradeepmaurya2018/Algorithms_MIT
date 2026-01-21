from fastapi import APIRouter, HTTPException
from pydantic import BaseModel

router = APIRouter()

class ShortenRequest(BaseModel):
    longUrl: str


def init_routes(service, server_id):

    @router.post("/shorten")
    def shorten(req: ShortenRequest):
        mapping = service.shorten(req.longUrl)
        print("Handled by PID:", server_id)
        return {
            "shortUrl": f"http://localhost/{mapping.short_code}",
            "handled_by_pid": server_id
        }

    @router.get("/{short_code}")
    def redirect(short_code: str):
        long_url = service.resolve(short_code)
        if not long_url:
            raise HTTPException(status_code=404, detail="URL not found")
        return {"redirect_to": long_url}

    return router
