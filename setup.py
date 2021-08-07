import setuptools
from setuptools import setup, Extension

with open("README.md", "r") as fh:
    long_description = fh.read()

module_ext = Extension('nicSpeedyModule', ['nic_speedy_module.c'])

setup(
     name='NicSpeedyModule',
     version='1.1.1',
     author="Lukas Mazl",
     author_email="lukasmazl586@gmail.com",
     description="Package for fetching network interface information",
     long_description=long_description,
     long_description_content_type="text/markdown",
     url="https://github.com/LukasMazl/NicSpeedyModule",
     project_urls={
        "Bug Tracker": "https://github.com/LukasMazl/NicSpeedyModule/issues",
     },
     packages=setuptools.find_packages(),
     classifiers=[
         "Development Status :: 4 - Beta",
         "License :: OSI Approved :: MIT License",
         "Operating System :: POSIX",
         "Operating System :: MacOS :: MacOS X",
         "Topic :: System :: Networking",
         "Topic :: System :: Operating System",
         "Programming Language :: Python :: 3",
         "Programming Language :: Python :: 3.5",
         "Programming Language :: Python :: 3.6",
         "Programming Language :: Python :: 3.7",
         "Programming Language :: Python :: 3.8",
         "Programming Language :: Python :: 3.9",

     ],
     python_requires=">=3.5",
     ext_modules=[module_ext]
 )