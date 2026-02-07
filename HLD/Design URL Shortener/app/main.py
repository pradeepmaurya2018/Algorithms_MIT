from fastapi import FastAPI
import os

from api.controller import init_routes
from services.shortner_service import UrlShortenerService
from repositories.url_repository_pg import UrlRepositoryPostgres
app = FastAPI(title="URL Shortener")

SERVER_ID = os.getpid()

repo = UrlRepositoryPostgres()
service = UrlShortenerService(repo)

app.include_router(init_routes(service, SERVER_ID))
