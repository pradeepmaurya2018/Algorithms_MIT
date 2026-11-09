# tests/test_l2_basic.py
from trafic.sniffer import Sniffer
from trafic.generator import TrafficGenerator
from core.testcase import TestCase
from scapy.all import Raw

class TestL2Basic(TestCase):
    name = "L2 Basic Frame"

    def run(self, ctx):
        gen = TrafficGenerator()
        sniff = Sniffer()

        pkt = gen.send_l2(
            iface=ctx.iface,
            dst_mac="ff:ff:ff:ff:ff:ff",
            payload_len=46
        )

        rx = sniff.capture(ctx.iface)
        return pkt, rx

    def verify(self, ctx, result):
        sent, received = result
        for pkt in received:
            if Raw in pkt and pkt[Raw].load == sent[Raw].load:
                return True, "Frame received correctly"
        return False, "Frame not observed"
