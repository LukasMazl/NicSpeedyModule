# NIC Speedy Module

[![Build Status](https://www.travis-ci.com/LukasMazl/NicSpeedyModule.svg?branch=master)](https://www.travis-ci.com/LukasMazl/NicSpeedyModule)

This repository were created for demonstrating of problem which appears in systems
with a lots of NICs. Let`s say you have a system with 1000 NIC. If you will use ethtool or netifaces,
then you will get results after several minutes. Why does it take too much time? Because this packages
do not have simple function for fetching all network`s interface information at the same time.

In /module_tests are created tests which measure total time of getting info about
system NICs. You can see that this module is faster then other, because it has one function which fetchs all information at the same time. You can measure that on your own.

## How to use this module?


 
