from scapy.all import Ether, IP, UDP, sendp

packet = (
    Ether(
        src="02:00:00:00:00:01",
        dst="ff:ff:ff:ff:ff:ff"   # broadcast
    )
    / IP(
        src="192.168.1.10",
        dst="192.168.1.255"
    )
    / UDP(
        sport=12345,
        dport=54321
    )
    / b"hello ethernet-pradeep says"
)
for i in range(10):
    sendp(packet, iface="eth0", count=1)
