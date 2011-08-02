# SConstruct file for a sample ARP poisoner

env = Environment()
env["CXXFLAGS"] = "-Wall -g -pthread -ggdb -std=c++0x"
env["LIBS"] = Split("cyanid net pcap pthread")
env["CPPPATH"] = Split("/usr/include/cyanid")

sources = Split("""
        src/main.cpp
        src/poisoner.cpp
        src/router.cpp
        src/basic_filter.cpp
        """)

env.Program('purgatory', sources)
