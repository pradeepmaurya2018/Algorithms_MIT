import socket, threading

users = {}  # user -> (ip, port, conn)

def handle_client(conn):
    try:
        while True:
            msg = conn.recv(4096).decode()
            if not msg:
                break

            print("\nRAW SIP MESSAGE:\n", msg)

            lines = msg.splitlines()
            if not lines:
                continue

            method, uri = lines[0].split(maxsplit=1)

            headers = {}
            for line in lines[1:]:
                if ": " in line:
                    k, v = line.split(": ", 1)
                    headers[k.strip()] = v.strip()

            if method == "REGISTER":
                user = headers.get("From")
                port = headers.get("Port")
                client_ip = conn.getpeername()[0]

                if not user or not port:
                    print("Malformed REGISTER:", headers)
                    continue

                users[user] = (client_ip, int(port), conn)
                print(f"{user} registered from {client_ip}:{port}")

            elif method == "INVITE":
                target = headers.get("To")
                if target and target in users:
                    users[target][2].send(msg.encode())

            elif method in ["RINGING", "OK", "BYE"]:
                target = headers.get("To")
                if target and target in users:
                    users[target][2].send(msg.encode())

    except Exception as e:
        print("Client error:", e)
    finally:
        conn.close()

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("0.0.0.0", 5060))
server.listen()

print("Mini SIP server running on 5060")

while True:
    conn, _ = server.accept()
    threading.Thread(target=handle_client, args=(conn,), daemon=True).start()
