cmake_minimum_required(VERSION 3.17)
project(SSC)

set(CMAKE_CXX_STANDARD 17)

add_executable(SSC main.cpp NFA.cpp NFA.h DFA.h DFA.cpp json.hpp ENFA.cpp ENFA.h RE.cpp RE.h)