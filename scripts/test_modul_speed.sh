#!/bin/bash

my_module_path="../module_tests/my_module_test.py"
netifaces_test_path="../module_tests/netifaces_test.py"
get_nic_test_path="../module_tests/get_nic_test.py"

separation_line="-------------------"

clear
echo "Testing my module"
  time python $my_module_path install >> /dev/null
echo $separation_line

echo "Testing netifaces"
  time python $netifaces_test_path >> /dev/null
echo $separation_line

echo "Testing get_nic"
  time python $get_nic_test_path >> /dev/null
echo $separation_line
