#!/usr/bin/env python3

from scooter import *
from scooter.gcc import build_gcc

here = dirof(__file__)

@build_main(here)
def _(b):
    sources = (
        here/'./see-codes.cpp',
    )
    
    include = (
    )
    
    opts = (
        '-O3',
        '-g',
        '-std=c++14',
        '-Wall',
        '-Wno-unused-variable',
        '-Wno-unused-result',
        '-Werror',
    )
    
    libs = (
    )
    
    cpp = 'g++'
    
    bin = here/'./see-codes'
    
    build_gcc(b, sources, bin, opts + include, gcc=cpp, libs=libs)

