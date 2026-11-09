from fastapi import FastAPI, HTTPException

from HLD.rest_apis.api.controllers import CreateBookRequest, CreateOrderRequest
from HLD.rest_apis.repositories.book_repository import BookRepository
from HLD.rest_apis.repositories.order_repository import OrderRepository
from HLD.rest_apis.services.book_service import BookService
from HLD.rest_apis.services.order_service import OrderService

app = FastAPI()

# Dependency wiring
book_repo = BookRepository()
order_repo = OrderRepository()

book_service = BookService(book_repo)
order_service = OrderService(book_repo, order_repo)

@app.post("/books")
def create_book(req: CreateBookRequest):
    return book_service.create_book(req.title, req.author, req.price)

@app.get("/books")
def list_books():
    return book_service.list_books()

@app.post("/orders")
def create_order(req: CreateOrderRequest):
    try:
        return order_service.place_order(req.book_id, req.quantity)
    except ValueError as e:
        raise HTTPException(status_code=404, detail=str(e))
