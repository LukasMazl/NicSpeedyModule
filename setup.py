from distutils.core import setup, Extension
setup(name='nicSpeedyModule', version='1.0', \
      ext_modules=[Extension('nicSpeedyModule', ['nic_speedy_module.c'])])
