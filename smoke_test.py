import pytest
import nicSpeedyModule
import socket

import netifaces


def test_get_all_if_by_AF():
    result = nicSpeedyModule.get_all_if_by_AF(socket.AF_PACKET)
    netifaces_result = netifaces.interfaces()
    assert len(result) == len(netifaces_result)


def test_get_ip():
    result = nicSpeedyModule.get_ip('lo')
    netifaces_result = netifaces.ifaddresses('lo')
    assert result == netifaces_result[2][0]['addr']

def test_get_all_ipv4_if():
    print(nicSpeedyModule.get_all_ipv4_if())

def test_get_info_if_by_AF():
    print(nicSpeedyModule.get_info_if_by_AF(socket.AF_INET))

def test_get_all_ipv6_if():
    print(nicSpeedyModule.get_all_ipv6_if())

def test_get_all_system_if():
    print(nicSpeedyModule.get_all_system_if())

def test_get_info_about_active_devices():
    print(nicSpeedyModule.get_info_about_active_devices())

def test_get_active_devices():
    print(nicSpeedyModule.get_active_devices())
