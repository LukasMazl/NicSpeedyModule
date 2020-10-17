#!/bin/bash

re='^[0-9]+$'
ipv4_re='^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\/[0-3]*[0-9]$'
interface_name_prefix="eth-test-"
ip="192.168.50.5/24"

if ! [[ $1 =~ $re ]] ; then
   echo "error: First parameter is null or is not number!"; exit 1
fi

if [[ -n $2 ]] ; then
    echo "Setting interface prefix to '$2'"
    interface_name_prefix=$2
fi

if [[ -n $3 ]] ; then
    if ! [[ $3 =~ $ipv4_re ]] ; then
      echo "error: Given IPv4 address '$3' is not valid"; exit 1
    fi
    echo "Default ip for interfaces is '$3'"
    ip=$3
fi

for i in $(seq 1 $1);
do
  interface_name=$interface_name_prefix$i
  ip link add $interface_name type dummy
  ip addr add $ip dev $interface_name
  ip link set $interface_name up
done;