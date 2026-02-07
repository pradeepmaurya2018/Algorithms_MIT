from scapy.all import sniff, ICMP, UDP

class TrafficReceiver:

    def recv_icmp(self, timeout=3):
        pkts = sniff(filter="icmp", timeout=timeout)
        return pkts

    def recv_udp(self, timeout=3):
        pkts = sniff(filter="udp", timeout=timeout)
        return pkts
