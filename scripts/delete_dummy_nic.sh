#!/bin/bash

re='^[0-9]+$'
interface_name_prefix="eth-test-"

if ! [[ $1 =~ $re ]] ; then
   echo "error: First parameter is null or is not number!"; exit 1
fi

if [[ -n $2 ]] ; then
    echo "Setting interface prefix to '$2'"
    interface_name_prefix=$2
fi

for i in $(seq 1 $1);
do
  interface_name=$interface_name_prefix$i
  ip link delete $interface_name type dummy
done;