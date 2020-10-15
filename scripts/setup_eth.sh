#!/bin/bash

for i in $(seq 1 750);
do
  ip link add eth-test-$i type dummy
  ip addr add 192.168.50.5/24 dev eth-test-$i
  ip link set eth-test-$i up
done;