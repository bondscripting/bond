#!/bin/bash

for toolname in "$@"
do
		mkdir "$toolname"
		cp prettyprint/prettyprint.vcxproj "$toolname/$toolname.vcxproj"
		cp prettyprint/prettyprint.vcxproj.filters "$toolname/$toolname.vcxproj.filters"
		sed -i "s/prettyprint/$toolname/" "$toolname/$toolname.vcxproj"
		sed -i "s/prettyprint/$toolname/" "$toolname/$toolname.vcxproj.filters"
done
