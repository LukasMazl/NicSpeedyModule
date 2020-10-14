#!/bin/bash

for i in $(seq 1 750);
do
  sudo ip addr add 192.168.50.5 dev eth-test-$i
done;
