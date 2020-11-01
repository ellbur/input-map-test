#!/usr/bin/env python3

from scooter import *
from scooter.gcc import build_gcc

here = dirof(__file__)

@build_main(here)
def _(b):
    sources = (
        here/'./main4.cpp',
        here/'./my-keyboard-layout-4.cpp',
    ) + (
        here/'../libtotalmap/src/' // '*.cpp'
    )
    
    include = (
        '-I' + (here/'..'),
        '-I' + (here/'../libtotalmap/include'),
        '-I' + (here/'../libtotalmap/third-party'),
    )
    
    opts = (
        '-O3',
        '-g',
        '-std=c++17',
        '-Wall',
        '-Wno-unused-variable',
        '-Wno-unused-result',
        '-Werror',
    )
    
    libs = (
        '-lboost_program_options',
    )
    
    cpp = 'g++'
    
    bin = here/'./map-keyboard-4'
    
    build_gcc(b, sources, bin, opts + include, gcc=cpp, libs=libs)

