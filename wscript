# wscript file for building purgatory

APPNAME = 'purgatory'
top     = '.'
out     = 'build'

cxxflags = ['-Wall', '-O2', '-std=c++0x', '-pedantic', '-D_GLIBCXX_USE_NANOSLEEP']
features = ['cxx', 'cxxprogram']
includes = '/usr/include/cyanid'
libs     = ['cyanid', 'pcap', 'pthread']

source = """ 
        src/main.cpp
        src/basic_filter.cpp
        src/poisoner.cpp
        src/router.cpp
        """

def options(opt):
    opt.load('compiler_cxx')

def configure(conf):
    conf.load('compiler_cxx')
    conf.check(features = features, cxxflags = cxxflags)

def build(bld):
    bld(features = features,
        cxxflags = cxxflags,
        source   = source, 
        includes = includes,
        lib      = libs,
        target   = APPNAME)
