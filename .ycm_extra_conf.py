
import os

flags = [
   '-std=c++17',
   '-x',
   'c++',
   '-Wall',
   '-Wextra',
   '-Werror',
   '-Wno-unknown-pragmas',
]

def IsHeaderFile(filename):
   extension = os.path.splitext(filename)[ 1 ]
   return extension in [ '.h', '.hxx', '.hpp', '.hh' ]

def GetCompilationInfoForFile(filename):
   return None

def FlagsForFile(filename, **kwargs):
   return {
       'flags': flags,
       'do_cache': True
   }

