import requests
BASE_URL = "http://127.0.0.1:8000/books"

book_data={
    "title":"clean_code",
    "author":"pradeep",
    "price":500
}
header={}

# req=requests.Request("POST", BASE_URL, json=book_data).prepare()

# print(req.url, req.headers, req.method, req.body)

response=requests.post(
    "http://127.0.0.1:8000/debug?name='pradeep'",
    json={"hello": "world"}
)

# print(response.status_code)
print(response.status_code)
# print(response.json())