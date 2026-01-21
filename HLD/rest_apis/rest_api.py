from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from typing import List
import uuid

app = FastAPI(title="Book Store API")

# ---------- Models ----------

class Book(BaseModel):
    id: str
    title: str
    author: str
    price: int

class CreateBook(BaseModel):
    title: str
    author: str
    price: int

class Order(BaseModel):
    order_id: str
    book_id: str
    quantity: int
    total_amount: int


# ---------- In-memory DB (for learning) ----------

books_db = {}
orders_db = {}


# ---------- Books APIs ----------

@app.get("/books", response_model=List[Book])
def get_books():
    return list(books_db.values())


@app.get("/books/{book_id}", response_model=Book)
def get_book(book_id: str):
    if book_id not in books_db:
        raise HTTPException(status_code=404, detail="Book not found")
    return books_db[book_id]


@app.post("/books", response_model=Book)
def create_book(book: CreateBook):
    book_id = str(uuid.uuid4())
    new_book = Book(id=book_id, **book.dict())
    books_db[book_id] = new_book
    return new_book


@app.delete("/books/{book_id}")
def delete_book(book_id: str):
    if book_id not in books_db:
        raise HTTPException(status_code=404, detail="Book not found")
    del books_db[book_id]
    return {"message": "Book deleted"}


# ---------- Orders APIs ----------

@app.post("/orders", response_model=Order)
def create_order(book_id: str, quantity: int):
    if book_id not in books_db:
        raise HTTPException(status_code=404, detail="Book not found")

    if quantity <= 0:
        raise HTTPException(status_code=400, detail="Invalid quantity")

    book = books_db[book_id]
    order_id = str(uuid.uuid4())
    total = book.price * quantity

    order = Order(
        order_id=order_id,
        book_id=book_id,
        quantity=quantity,
        total_amount=total
    )

    orders_db[order_id] = order
    return order


@app.get("/orders/{order_id}", response_model=Order)
def get_order(order_id: str):
    if order_id not in orders_db:
        raise HTTPException(status_code=404, detail="Order not found")
    return orders_db[order_id]
