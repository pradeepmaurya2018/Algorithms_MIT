# core/testcase.py
class TestCase:
    name = "base"

    def setup(self, ctx):
        pass

    def run(self, ctx):
        raise NotImplementedError

    def verify(self, ctx, result):
        raise NotImplementedError

    def teardown(self, ctx):
        pass
