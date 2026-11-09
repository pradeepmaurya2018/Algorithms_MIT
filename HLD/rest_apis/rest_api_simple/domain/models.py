from dataclasses import dataclass

@dataclass
class Book:
    id: str
    title: str
    author: str
    price: int


@dataclass
class Order:
    id: str
    book_id: str
    quantity: int
    total_amount: int
