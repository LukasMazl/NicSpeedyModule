#!/bin/bash

for i in $(seq 1 750);
do
  ip link add eth-test-$i type dummy
done;