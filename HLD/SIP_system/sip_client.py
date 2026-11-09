import socket
import time
SERVER = ("127.0.0.1", 5060)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(SERVER)

def send(msg):
    sock.send(msg.encode())

def register(user, ip, port):
        send(f"""REGISTER sip:{user}
        From: {user}
        IP: {ip}
        Port: {port}
        """)

def invite(from_user, to_user, ip, port):
    send(f"""INVITE sip:{to_user}
        From: {from_user}
        To: {to_user}
        IP: {ip}
        Port: {port}
        """)


time.sleep(5)
register("alice", "127.0.0.1", 5001)
time.sleep(5)
register("bob", "127.0.0.1", 5002)

invite("alice", "bob", "127.0.0.1", 5003)
time.sleep(5)
send("""RINGING sip:alice
From: bob
To: alice
""")