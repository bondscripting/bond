#!/bin/bash

for testname in "$@"
do
		mkdir "$testname"
		cp prettyprint/prettyprint.vcxproj "$testname/$testname.vcxproj"
		cp prettyprint/prettyprint.vcxproj.filters "$testname/$testname.vcxproj.filters"
		sed -i "s/prettyprint/$testname/" "$testname/$testname.vcxproj"
		sed -i "s/prettyprint/$testname/" "$testname/$testname.vcxproj.filters"
done
