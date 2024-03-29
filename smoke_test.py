import pytest
import nicSpeedyModule
import socket



def test_get_all_if_by_AF():
    result = nicSpeedyModule.get_all_if_by_AF(socket.AF_INET6)
    print (result)

def test_get_all_ipv4_if():
    print(nicSpeedyModule.get_all_ipv4_if())

def test_get_info_if_by_AF():
    print(nicSpeedyModule.get_info_if_by_AF(socket.AF_INET))

def test_get_all_ipv6_if():
    print(nicSpeedyModule.get_all_ipv6_if())

def test_get_all_system_if():
    print(nicSpeedyModule.get_all_system_if())

def test_get_info_about_active_devices():
    print(nicSpeedyModule.get_info_about_active_interfaces())

def test_get_active_devices():
    print(nicSpeedyModule.get_info_about_active_interfaces())
