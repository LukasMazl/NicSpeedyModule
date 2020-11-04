import my_module
import socket

devices = my_module.get_info_if_by_AF(socket.AF_INET)
devices = my_module.get_ip('lo')
devices = my_module.get_all_system_if()
