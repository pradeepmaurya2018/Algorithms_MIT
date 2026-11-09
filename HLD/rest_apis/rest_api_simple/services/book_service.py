import uuid

from HLD.rest_apis.rest_api_backup import Book


class BookService:
    def __init__(self, repository):
        self.repository = repository

    def create_book(self, title, author, price):
        book = Book(
            id=str(uuid.uuid4()),
            title=title,
            author=author,
            price=price
        )
        self.repository.save(book)
        return book

    def list_books(self):
        return self.repository.get_all()

    def get_book(self, book_id):
        return self.repository.get(book_id)
