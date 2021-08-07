# NIC Speedy Module


|   OS  	|                                                                        Status                                                                        	|
|:-----:	|:----------------------------------------------------------------------------------------------------------------------------------------------------:	|
| Linux 	| [![Build Status](https://www.travis-ci.com/LukasMazl/NicSpeedyModule.svg?branch=master)](https://www.travis-ci.com/LukasMazl/NicSpeedyModule)        	|
|  OsX  	| [![Build Status](https://www.travis-ci.com/LukasMazl/NicSpeedyModule.svg?branch=master)](https://www.travis-ci.com/LukasMazl/NicSpeedyModule)        	|

This repository was created for demonstrating of problem which appears in systems
with a lots of NICs. Lets say you have a system with 1000 NIC. If you will use ethtool or netifaces,
then you will get results after several minutes. Why does it take too much time? Because this packages
do not have simple function for fetching all network`s interface information at the same time.

In /module_tests are created tests which measure total time of getting info about
system NICs. You can see that this module is faster then other, because it has one function which fetchs all information at the same time. You can measure that on your own.

## About module
This module was created for simple reading informtion about network interface. In the results of module function you may see ,,magic" numbers like flags or family.
I would like you show you how to ,,read" these magic numbers on example samples below. Below you can also see examples how you may use module functions.

### Which information does this module return?
NicSpeedyModule uses system library getifaddrs.h. Results from the libary are transformed to pyObject which you may use later. This module returns:

|     Name     	|   Type  	|                 Example                 	|
|:------------:	|:-------:	|:---------------------------------------:	|
|     name     	|  String 	|             eth0, lo0, ipsec            	|
|    family    	| Integer 	|         2(AF_INET), 30(AF_INET6)        	|
|      up      	| Boolean 	|                True/False               	|
|     flags    	| Integer 	|                  32841                  	|
|     addr     	|  String 	|              127.0.0.1, ::1             	|
| addr_netmask 	|  String 	| 255.255.255.0,<br>ffff:ffff:ffff:ffff:: 	|

 
### Transform family to AddrFamily type
Because value family is integer type, so it looks like a magic number, but its not. This number represent Address Family type.
Here is simple example how you can get from magic number Addres Family type.

```python
import nicSpeedyModule as nic
import socket

results = nic.get_them_all()
interface = results["lo0"][0] # Take first result from list

addr_family_type = socket.AddressFamily(interface["family"])
print(addr_family_type)
```



### Flags description
This module allows to access to interface flags, which could be used for determining whether is interface up or whether support multicast etc...
    
    Device flags
    IFF_UP            Interface is running.
    IFF_BROADCAST     Valid broadcast address set.
    IFF_DEBUG         Internal debugging flag.
    IFF_LOOPBACK      Interface is a loopback interface.
    IFF_POINTOPOINT   Interface is a point-to-point link.
    IFF_RUNNING       Resources allocated.
    IFF_NOARP         No arp protocol, L2 destination address not set.
    IFF_PROMISC       Interface is in promiscuous mode.
    IFF_NOTRAILERS    Avoid use of trailers.
    IFF_ALLMULTI      Receive all multicast packets.
    IFF_MASTER        Master of a load balancing bundle.
    IFF_SLAVE         Slave of a load balancing bundle.
    IFF_MULTICAST     Supports multicast
    IFF_PORTSEL       Is able to select media type via ifmap.
    IFF_AUTOMEDIA     Auto media selection active.
    IFF_DYNAMIC       The addresses are lost when the interface goes down.
    IFF_LOWER_UP      Driver signals L1 up (since Linux 2.6.17)
    IFF_DORMANT       Driver signals dormant (since Linux 2.6.17)
    IFF_ECHO          Echo sent packets (since Linux 2.6.25)

(Taken from https://man7.org/linux/man-pages/man7/netdevice.7.html)

Here is a simple example how to determine whether is it loopback interface.

```python
import nicSpeedyModule as nic
results = nic.get_them_all()
interface = results["lo0"][0] # Take first result from list


is_loopback = (interface["flags"] & nic.IFF_LOOPBACK) != 0
print(is_loopback)
```



