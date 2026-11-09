class BookRepository:
    def __init__(self):
        self._books = {}

    def save(self, book):
        self._books[book.id] = book

    def get(self, book_id):
        return self._books.get(book_id)

    def get_all(self):
        return list(self._books.values())
