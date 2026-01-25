# =========================================================
# DATA COMMUNICATION & COMPUTER NETWORKS SIMULATOR
# Shows packet at EVERY layer
# =========================================================


# -------------------------
# USER SPACE
# -------------------------

class Application:
    def __init__(self, socket):
        self.socket = socket

    def send(self, data):
        print("\n=== APPLICATION LAYER (USER SPACE) ===")
        print("Data:", data)
        self.socket.send(data)

    def receive(self, data):
        print("\n=== APPLICATION RECEIVES DATA ===")
        print("Data:", data)


class Socket:
    def __init__(self, kernel):
        self.kernel = kernel

    def send(self, data):
        print("\n--- System Call: send() ---")
        self.kernel.transport_layer(data)

    def deliver_to_app(self, data):
        app.receive(data)


# -------------------------
# KERNEL SPACE
# -------------------------

class Kernel:
    def __init__(self, ip, mac, router):
        self.ip = ip
        self.mac = mac
        self.router = router

    # -------- Transport Layer (TCP) --------
    def transport_layer(self, app_data):
        tcp_segment = {
            "src_port": 50000,
            "dst_port": 80,
            "seq": 1001,
            "ack": 0,
            "data": app_data
        }

        print("\n=== TRANSPORT LAYER (TCP) ===")
        print(tcp_segment)

        self.network_layer(tcp_segment)

    # -------- Network Layer (IP) --------
    def network_layer(self, tcp_segment):
        ip_packet = {
            "src_ip": self.ip,
            "dst_ip": "93.184.216.34",  # example.com
            "ttl": 64,
            "payload": tcp_segment
        }

        print("\n=== NETWORK LAYER (IP) ===")
        print(ip_packet)

        self.data_link_layer(ip_packet)

    # -------- Data Link Layer (Ethernet) --------
    def data_link_layer(self, ip_packet):
        ethernet_frame = {
            "src_mac": self.mac,
            "dst_mac": "AA:BB:CC:DD:EE:FF",  # router MAC
            "ethertype": "IPv4",
            "payload": ip_packet
        }

        print("\n=== DATA LINK LAYER (ETHERNET) ===")
        print(ethernet_frame)

        self.physical_layer(ethernet_frame)

    # -------- Physical Layer --------
    def physical_layer(self, frame):
        print("\n=== PHYSICAL LAYER ===")
        print("0101010100110101... (signals on wire/air)")

        # Send frame to router
        self.router.receive(frame)

    # -------- Receive Path --------
    def receive_from_network(self, ip_packet):
        print("\n=== KERNEL RECEIVING PACKET ===")
        print(ip_packet)

        tcp_segment = ip_packet["payload"]
        app_data = tcp_segment["data"]

        socket.deliver_to_app(app_data)


# -------------------------
# ROUTER (NETWORK)
# -------------------------

class Router:
    def receive(self, frame):
        print("\n=== ROUTER RECEIVES FRAME ===")
        print(frame)

        # Strip Ethernet
        ip_packet = frame["payload"]
        ip_packet["ttl"] -= 1

        print("\n=== ROUTER FORWARDS IP PACKET ===")
        print(ip_packet)

        server.receive(ip_packet)


# -------------------------
# SERVER (DESTINATION)
# -------------------------

class Server:
    def receive(self, ip_packet):
        print("\n=== SERVER RECEIVES IP PACKET ===")
        print(ip_packet)

        tcp_segment = ip_packet["payload"]
        request = tcp_segment["data"]

        print("\n=== SERVER APPLICATION ===")
        print("Request:", request)

        response_data = f"HTTP/1.1 200 OK | Response to '{request}'"

        response_segment = {
            "src_port": 80,
            "dst_port": tcp_segment["src_port"],
            "seq": 2001,
            "ack": tcp_segment["seq"] + 1,
            "data": response_data
        }

        response_packet = {
            "src_ip": ip_packet["dst_ip"],
            "dst_ip": ip_packet["src_ip"],
            "ttl": 64,
            "payload": response_segment
        }

        print("\n=== SERVER SENDS RESPONSE ===")
        print(response_packet)

        kernel.receive_from_network(response_packet)


# -------------------------
# BOOTSTRAP SYSTEM
# -------------------------

router = Router()
kernel = Kernel(
    ip="192.168.1.10",
    mac="11:22:33:44:55:66",
    router=router
)

socket = Socket(kernel)
app = Application(socket)
server = Server()

# -------------------------
# RUN SIMULATION
# -------------------------

app.send("GET /index.html HTTP/1.1")
