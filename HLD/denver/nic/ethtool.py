# nic/ethtool.py
import subprocess

def get_stats(iface):
    out = subprocess.check_output(["ethtool", "-S", iface])
    return out.decode()

def set_offload(iface, feature, state):
    subprocess.run(["ethtool", "-K", iface, feature, state])
