from setuptools import setup, find_packages
from setuptools.extension import Extension
from Cython.Build import cythonize

import os

current_dir = os.path.dirname(os.path.realpath(__file__))

setup(name='pngpack',
      version='1.0.0',
      url='https://github.com/axiom-data-science/pngpack/',
      packages=find_packages(),
      ext_modules=cythonize(
          [
              Extension("pngpack",
                        sources=["pngpack.pyx", "../libpngpack/pngpack.c"],
                        libraries=["png"],
                        library_dirs=["~/dev/opt/homebrew/opt/libpng/lib/"]),
          ],
          compiler_directives={'language_level': '3'},
      ),
      zip_safe=False,
      classifiers=[
          "Programming Language :: Python :: 3",
          "License :: OSI Approved :: MIT License",
      ])
