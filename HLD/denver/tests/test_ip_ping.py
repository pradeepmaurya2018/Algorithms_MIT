import uuid

from core.testcase import TestCase
from storage.db import PacketDB
from traffic.generator import TrafficGenerator
from traffic.metadata import packet_to_record


class TestIPPing(TestCase):
    name = "IP_ICMP_PING"

    def run(self, ctx):
        run_id = uuid.uuid4()
        gen = TrafficGenerator()
        db = PacketDB()

        sent, reply = gen.send_icmp(ctx.target_ip)

        records = []
        records.append(packet_to_record(
            sent, "tx", self.name, run_id
        ))

        if reply:
            records.append(packet_to_record(
                reply, "rx", self.name, run_id
            ))

        db.insert_packets(records)
        return reply

    def verify(self, ctx, reply):
        if reply:
            return True, "ICMP reply received"
        return False, "No ICMP reply"
