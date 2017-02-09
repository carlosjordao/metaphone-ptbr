from distutils.core import setup, Extension
from os.path import join

c_ext = Extension("metaphoneptbr", ["metaphone_ptbrpy.c", join("..", "source", "metaphone_ptbr.c")])

setup(
    name='Metaphone-ptbr',
    version='1.15',
    ext_modules=[c_ext],
    include_dirs=[".", join("..", "source")]
)
