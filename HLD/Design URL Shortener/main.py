from fastapi import FastAPI
import os

from api.controller import init_routes
from repositories.url_repository import UrlRepository
from services.shortner_service import UrlShortenerService

app = FastAPI(title="URL Shortener")

SERVER_ID = os.getpid()

repo = UrlRepository()
service = UrlShortenerService(repo)

app.include_router(init_routes(service, SERVER_ID))
