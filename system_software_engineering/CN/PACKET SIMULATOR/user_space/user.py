# =========================
# USER SPACE
# =========================

class Application:
    def __init__(self, name, socket):
        self.name = name
        self.socket = socket

    def send_request(self, data):
        print(f"[USER] {self.name} sending data: {data}")
        self.socket.send(data)

    def receive_response(self, data):
        print(f"[USER] {self.name} received response: {data}")


# =========================
# SOCKET (USER → KERNEL)
# =========================

class Socket:
    def __init__(self, kernel):
        self.kernel = kernel

    def send(self, data):
        # system call boundary
        self.kernel.tcp_send(data)

    def deliver_to_user(self, data):
        app.receive_response(data)