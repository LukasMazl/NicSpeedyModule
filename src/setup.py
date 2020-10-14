from distutils.core import setup, Extension
setup(name='my_module', version='1.0', \
      ext_modules=[Extension('my_module', ['my_module.c'])])

import my_module


my_module.show_if()
devices = my_module.get_all_if()
print(devices)
for device in devices:
      print(device["name"])
print("\n")