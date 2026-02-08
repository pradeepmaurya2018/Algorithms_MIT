import uuid

from HLD.rest_apis.rest_api_backup import Order


class OrderService:
    def __init__(self, book_repo, order_repo):
        self.book_repo = book_repo
        self.order_repo = order_repo

    def place_order(self, book_id, quantity):
        book = self.book_repo.get(book_id)
        if not book:
            raise ValueError("BOOK_NOT_FOUND")

        total = book.price * quantity

        order = Order(
            id=str(uuid.uuid4()),
            book_id=book_id,
            quantity=quantity,
            total_amount=total
        )

        self.order_repo.save(order)
        return order
