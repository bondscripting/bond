#!/bin/bash

for testname in "$@"
do
		mkdir "$testname"
		cp testlexer/testlexer.vcxproj "$testname/$testname.vcxproj"
		cp testlexer/testlexer.vcxproj.filters "$testname/$testname.vcxproj.filters"
		sed -i "s/testlexer/$testname/" "$testname/$testname.vcxproj"
		sed -i "s/testlexer/$testname/" "$testname/$testname.vcxproj.filters"
done
