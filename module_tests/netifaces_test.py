import netifaces

addrs = []
interfaces = netifaces.interfaces()
for interface in interfaces:
    addrs.append(netifaces.ifaddresses(interface))


