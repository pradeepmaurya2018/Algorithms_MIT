from trafic.generator import TrafficGenerator
from core.testcase import TestCase

class TestIPPing(TestCase):
    name = "IP ICMP Ping"

    def run(self, ctx):
        gen = TrafficGenerator()
        sent, reply = gen.send_icmp(ctx.target_ip)
        return sent, reply

    def verify(self, ctx, result):
        sent, reply = result
        if reply and reply.src == ctx.target_ip:
            return True, "ICMP reply received"
        return False, "No ICMP reply"
