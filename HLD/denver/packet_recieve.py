from scapy.all import sniff, Ether, IP, UDP

counter=0
packets=[]
def handle_packet(pkt):
    global counter
    packets.append(pkt[Ether])
    # if pkt.haslayer(Ether):
    #     print("Ethernet:")
    #     print("  src:", pkt[Ether].src)
    #     print("  dst:", pkt[Ether].dst)

    # if pkt.haslayer(IP):
    #     print("IP:")
    #     print("  src:", pkt[IP].src)
    #     print("  dst:", pkt[IP].dst)

    # if pkt.haslayer(UDP):
    #     print("UDP:")
    #     print("  sport:", pkt[UDP].sport)
    #     print("  dport:", pkt[UDP].dport)

    # if pkt.haslayer(UDP) and bytes(pkt[UDP].payload):
    #     print("Payload:", bytes(pkt[UDP].payload))
    #     print("-" * 40)
    counter+=1
    # print(f"Total packet  received {counter}")

sniff(
    iface="eth0",
    prn=handle_packet,
    filter="udp port 54321",
    store=False
)
for packet in packets:
    print(packet)
