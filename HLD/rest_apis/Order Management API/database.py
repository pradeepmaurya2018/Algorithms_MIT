users_db = {
    "user1": {
        "id": 1,
        "username": "user1",
        "hashed_password": "$2b$12$KIXQ4Y0...",  # bcrypt hash
        "role": "USER"
    },
    "admin": {
        "id": 2,
        "username": "admin",
        "hashed_password": "$2b$12$KIXQ4Y0...",
        "role": "ADMIN"
    }
}

orders_db = [
    {"id": 1, "owner_id": 1, "item": "Book"},
    {"id": 2, "owner_id": 2, "item": "Laptop"}
]
