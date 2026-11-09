from fastapi import FastAPI, Depends, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from slowapi import Limiter
from slowapi.util import get_remote_address

from models import LoginRequest, OrderCreate
from security import verify_password, create_access_token
from database import users_db, orders_db
from auth import get_current_user, require_admin

app = FastAPI()

# CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["https://myfrontend.com"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Rate limiting
limiter = Limiter(key_func=get_remote_address)
app.state.limiter = limiter

@app.post("/login")
@limiter.limit("5/minute")
def login(data: LoginRequest):
    user = users_db.get(data.username)
    if not user or not verify_password(data.password, user["hashed_password"]):
        raise HTTPException(status_code=401, detail="Invalid credentials")

    token = create_access_token({
        "sub": user["username"],
        "role": user["role"]
    })
    return {"access_token": token, "token_type": "bearer"}

@app.get("/orders")
def get_orders(user=Depends(get_current_user)):
    if user["role"] == "ADMIN":
        return orders_db
    return [o for o in orders_db if o["owner_id"] == user["id"]]

@app.post("/orders")
def create_order(order: OrderCreate, user=Depends(get_current_user)):
    new_order = {
        "id": len(orders_db) + 1,
        "owner_id": user["id"],
        "item": order.item
    }
    orders_db.append(new_order)
    return new_order

@app.get("/admin/orders")
def admin_orders(admin=Depends(require_admin)):
    return orders_db
