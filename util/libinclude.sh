#!/bin/bash

prettyprint="$PWD/build/bin/prettyprint"
embedfile="$PWD/build/util/embedfile"
hFile="$PWD/include/bond/private/libinclude_embedded.h"
cppFile="$PWD/source/private/libinclude_embedded_cpp.h"
inputDir="lib/include"
buildDir="build/libinclude"

make prettyprint
make embedfile

mkdir -p "$buildDir"

for file in "$inputDir"/*.bond
do
	"$prettyprint" -m "$file" > "$buildDir/include_"$(basename "$file")
done

cd "$buildDir"
"$embedfile" -h ../../include/bond/private/libinclude_embedded.h -c ../../source/private/libinclude_embedded_cpp.h *.bond
