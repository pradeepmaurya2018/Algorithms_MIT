import time
from scapy.layers.inet import IP, TCP, UDP

def packet_to_record(pkt, direction, test_name, run_id):
    record = {
        "ts": time.time(),
        "src_ip": None,
        "dst_ip": None,
        "protocol": None,
        "src_port": None,
        "dst_port": None,
        "packet_size": len(pkt),
        "direction": direction,
        "test_name": test_name,
        "run_id": run_id
    }

    if IP in pkt:
        record["src_ip"] = pkt[IP].src
        record["dst_ip"] = pkt[IP].dst
        record["protocol"] = pkt[IP].proto

        if TCP in pkt:
            record["src_port"] = pkt[TCP].sport
            record["dst_port"] = pkt[TCP].dport
            record["protocol"] = "TCP"

        elif UDP in pkt:
            record["src_port"] = pkt[UDP].sport
            record["dst_port"] = pkt[UDP].dport
            record["protocol"] = "UDP"

        else:
            record["protocol"] = "IP"

    return record
