from setuptools import Extension, setup
from os.path import join

setup(
    name='metaphone-ptbr',
    version='1.23',
    ext_modules=[
        Extension(name="metaphoneptbr", 
                  sources=["metaphone_ptbrpy.c", join("..", "source", "metaphone_ptbr.c")])
    ],
    include_dirs=[".", join("..", "source")]
)
