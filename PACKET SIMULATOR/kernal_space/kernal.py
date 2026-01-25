# =========================
# KERNEL SPACE
# =========================

class Kernel:
    def __init__(self, ip, mac, router):
        self.ip = ip
        self.mac = mac
        self.router = router

    # ---- Transport Layer ----
    def tcp_send(self, data):
        segment = {
            "src_port": 50000,
            "dst_port": 80,
            "data": data
        }
        print("[KERNEL] TCP segment created")
        self.ip_send(segment)

    # ---- Network Layer ----
    def ip_send(self, segment):
        packet = {
            "src_ip": self.ip,
            "dst_ip": "93.184.216.34",  # example.com
            "payload": segment
        }
        print("[KERNEL] IP packet created")
        self.ethernet_send(packet)

    # ---- Data Link Layer ----
    def ethernet_send(self, packet):
        frame = {
            "src_mac": self.mac,
            "dst_mac": "AA:BB:CC:DD:EE:FF",  # router MAC
            "payload": packet
        }
        print("[KERNEL] Ethernet frame created")
        self.router.forward(frame)

    # ---- Receive path ----
    def receive_from_network(self, packet):
        print("[KERNEL] Packet received from network")
        segment = packet["payload"]
        data = segment["data"]
        socket.deliver_to_user(data)
