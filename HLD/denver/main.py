from tests.test_ip_ping import TestIPPing
from core.verdict import verdict

class Context:
    def __init__(self, target_ip):
        self.target_ip = target_ip

if __name__ == "__main__":
    ctx = Context(target_ip="8.8.8.8")  # or another machine
    test = TestIPPing()

    test.setup(ctx)
    result = test.run(ctx)
    passed, reason = test.verify(ctx, result)
    verdict(test, passed, reason)
    test.teardown(ctx)
