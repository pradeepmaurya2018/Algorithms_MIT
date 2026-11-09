from pymongo import MongoClient

client = MongoClient("mongodb://localhost:27017/")
db = client["todo_app"]
tasks = db["tasks"]

tasks.insert_many([
    {"title": "Learn MongoDB", "completed": False, "priority": 2},
    {"title": "Build toy app", "completed": False, "priority": 1},
])

print("\nTasks sorted by priority:")
for task in tasks.find().sort("priority", 1):
    print(task)

tasks.update_one({"priority": 1}, {"$set": {"completed": True}})

print("\nAfter update:")
for task in tasks.find():
    print(task)
