from scapy.all import *

class Sniffer:
    def capture(self, iface, timeout=2):
        return sniff(
            iface=iface,
            timeout=timeout,
            store=True,
            filter=None
        )
