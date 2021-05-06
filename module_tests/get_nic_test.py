from get_nic import getnic

interfaces = getnic.interfaces()
print(getnic.ipaddr(interfaces))