from scapy.all import IP, ICMP, UDP, Raw, send, sr1

class TrafficGenerator:

    def send_icmp(self, dst_ip):
        pkt = IP(dst=dst_ip) / ICMP()
        reply = sr1(pkt, timeout=2, verbose=False)
        return pkt, reply

    def send_udp(self, dst_ip, dport, payload=b"hello pradeep"):
        pkt = IP(dst=dst_ip) / UDP(dport=dport) / Raw(payload)
        send(pkt, verbose=False)
        return pkt
