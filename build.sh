#!/bin/bash

g++ -Wall -Iinclude -g $(find source -name '*.cpp') test/testmain.cpp -o testmain
