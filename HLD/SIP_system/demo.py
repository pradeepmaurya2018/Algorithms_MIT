import socket
import threading
import pyaudio
import sys
import time

# ===================== CONFIG =====================
SIGNAL_PORT = 5060
AUDIO_PORT = 5004

RATE = 16000
CHUNK = 320          # 20ms
FORMAT = pyaudio.paInt16
CHANNELS = 1

# ===================== AUDIO INIT =====================
pa = pyaudio.PyAudio()
mic = None
speaker = None
AUDIO_ENABLED = True

def init_audio():
    global mic, speaker, AUDIO_ENABLED

    try:
        mic = pa.open(
            format=FORMAT,
            channels=CHANNELS,
            rate=RATE,
            input=True,
            frames_per_buffer=CHUNK,
            input_device_index=None
        )

        speaker = pa.open(
            format=FORMAT,
            channels=CHANNELS,
            rate=RATE,
            output=True,
            frames_per_buffer=CHUNK,
            output_device_index=None
        )

        print("[AUDIO] Microphone and speaker initialized")

    except OSError as e:
        print("[AUDIO] No usable audio device found")
        print("[AUDIO] Running in SILENT MODE")
        AUDIO_ENABLED = False


init_audio()

# ===================== AUDIO HELPERS =====================
def read_audio():
    if not AUDIO_ENABLED:
        return b'\x00' * CHUNK * 2
    return mic.read(CHUNK, exception_on_overflow=False)

def play_audio(data):
    if AUDIO_ENABLED:
        speaker.write(data)

# ===================== SIGNAL SERVER =====================
def start_server():
    users = {}

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(("0.0.0.0", SIGNAL_PORT))
    server.listen()

    print("[SERVER] Signaling server running")

    def handle_client(conn):
        name = None
        while True:
            try:
                msg = conn.recv(1024).decode().strip()
                if not msg:
                    break

                if msg.startswith("REGISTER"):
                    name = msg.split()[1]
                    users[name] = conn
                    print(f"[SERVER] {name} registered")

                elif msg.startswith("CALL"):
                    target = msg.split()[1]
                    if target in users:
                        users[target].send(f"INCOMING {name}\n".encode())

                elif msg.startswith("ACCEPT"):
                    caller = msg.split()[1]
                    if caller in users:
                        users[caller].send(b"ACCEPTED\n")

            except:
                break

        if name and name in users:
            del users[name]
        conn.close()

    while True:
        conn, _ = server.accept()
        threading.Thread(target=handle_client, args=(conn,), daemon=True).start()

# ===================== AUDIO STREAMING =====================
def send_audio(target_ip):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    while True:
        data = read_audio()
        sock.sendto(data, (target_ip, AUDIO_PORT))

def receive_audio():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", AUDIO_PORT))
    while True:
        data, _ = sock.recvfrom(2048)
        play_audio(data)

# ===================== CLIENT =====================
def start_client(server_ip, name):
    signal = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    signal.connect((server_ip, SIGNAL_PORT))
    signal.send(f"REGISTER {name}\n".encode())

    print(f"[CLIENT] Registered as {name}")

    def listen_signaling():
        while True:
            msg = signal.recv(1024).decode().strip()
            if msg.startswith("INCOMING"):
                caller = msg.split()[1]
                print(f"[CALL] Incoming from {caller}")
                signal.send(f"ACCEPT {caller}\n".encode())

            elif msg == "ACCEPTED":
                print("[CALL] Call accepted")

    threading.Thread(target=listen_signaling, daemon=True).start()

    print("Commands:")
    print("  call <peer_ip>")
    print("  exit")

    while True:
        cmd = input("> ").strip()
        if cmd.startswith("call"):
            peer_ip = cmd.split()[1]
            print("[CALL] Starting audio stream")
            threading.Thread(target=send_audio, args=(peer_ip,), daemon=True).start()
            threading.Thread(target=receive_audio, daemon=True).start()

        elif cmd == "exit":
            break

# ===================== MAIN =====================
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python voip_wslg.py server")
        print("  python voip_wslg.py client <server_ip> <name>")
        sys.exit(1)

    if sys.argv[1] == "server":
        start_server()

    elif sys.argv[1] == "client":
        start_client(sys.argv[2], sys.argv[3])
