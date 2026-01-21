from fastapi import APIRouter, HTTPException
from pydantic import BaseModel

router = APIRouter()

class CreateBookRequest(BaseModel):
    title: str
    author: str
    price: int

class CreateOrderRequest(BaseModel):
    book_id: str
    quantity: int
