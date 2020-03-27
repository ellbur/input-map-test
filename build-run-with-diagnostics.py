#!/usr/bin/env python3

from scooter import *
from scooter.gcc import build_gcc

here = dirof(__file__)

@build_main(here)
def _(b):
    sources = (
        here/'./run-with-diagnostics.cpp',
    )
    
    include = (
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
    
    bin = here/'./run-with-diagnostics'
    
    build_gcc(b, sources, bin, opts + include, gcc=cpp, libs=libs)

