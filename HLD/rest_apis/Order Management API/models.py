from pydantic import BaseModel, Field

class LoginRequest(BaseModel):
    username: str = Field(min_length=3, max_length=50)
    password: str = Field(min_length=6, max_length=72)


class OrderCreate(BaseModel):
    item: str = Field(min_length=1, max_length=50)
