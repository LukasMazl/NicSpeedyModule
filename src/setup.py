from distutils.core import setup, Extension
setup(name='my_module', version='1.0', \
      ext_modules=[Extension('my_module', ['my_module.c'])])