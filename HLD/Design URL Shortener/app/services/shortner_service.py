from domain.model import UrlMapping
from utils.encoder import generate_short_code


class UrlShortenerService:
    def __init__(self, repository):
        self.repository = repository

    def shorten(self, long_url: str):
        while True:
            code = generate_short_code()
            if not self.repository.find(code):
                mapping = UrlMapping(code, long_url)
                self.repository.save(mapping)
                return mapping

    def resolve(self, short_code: str):
        return self.repository.find(short_code)
