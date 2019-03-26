from setuptools import setup, find_packages
from setuptools.extension import Extension
from Cython.Build import cythonize

import os

current_dir = os.path.dirname(os.path.realpath(__file__))

setup(name='pngpack',
      version='1.0.0',
      packages=find_packages(),
      ext_modules=cythonize(Extension("pngpack",
                                      sources=["pngpack.pyx"],
                                      libraries=["pngpack"],
                                      library_dirs=[current_dir + "/../libpngpack/cmake-build-release"])),
      zip_safe=False)
