#!/bin/bash

my_module_path="../module_tests/my_module_test.py"
netifaces_test_path="../module_tests/netifaces_test.py"
get_nic_test_path="../module_tests/get_nic_test.py"
ifaddr_test_path="../module_tests/ifaddr_test.py"
getNifs_test_path="../module_tests/getnifs/getnifs_test.py"

separation_line="-------------------"

clear
echo "Testing my module"
  time python3 $my_module_path install >> /dev/null
echo $separation_line

echo "Testing netifaces"
  time python3 $netifaces_test_path >> /dev/null
echo $separation_line

echo "Testing get_nic"
  time python3 $get_nic_test_path >> /dev/null
echo $separation_line

echo "Testing ifaddr"
  time python3 $ifaddr_test_path >> /dev/null
echo $separation_line

echo "Testing getNifs"
  time python3 $getNifs_test_path >> /dev/null
echo $separation_line