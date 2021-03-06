from setuptools import setup, find_packages
from setuptools.extension import Extension
from Cython.Build import cythonize

import os

current_dir = os.path.dirname(os.path.realpath(__file__))

setup(name='pngpack-py',
      version='1.0.0',
      url='https://github.com/axiom-data-science/pngpack/',
      author='Axiom Data Science',
      author_email='support+pngpack@axiomdatascience.com',
      description="library to pack floating point values into 16-bit PNG images",
      packages=[],
      ext_modules=cythonize(
          [
              Extension("pngpack",
                        sources=["pngpack.pyx", "../libpngpack/pngpack.c"],
                        define_macros=[('_GNU_SOURCE', None)],
                        libraries=["png"],
                        ),
          ],
          compiler_directives={'language_level': '3'},
      ),
      zip_safe=False,
      classifiers=[
          "Programming Language :: Python :: 3",
          "License :: OSI Approved :: MIT License",
      ])
