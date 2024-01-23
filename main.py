def dec_factory(x):
    def decorator(func):
        def wrapped(*args, **kwargs):
            print(x)
            return func(*args, **kwargs)
        return wrapped
    return decorator

class Thing:
    y = 71
    @dec_factory(y)
    def method(self):
        print("sup")

Thing().method()