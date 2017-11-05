#!/bin/bash

bondc="$PWD/build/bin/bondc"
embedfile="$PWD/build/util/embedfile"
hFile="$PWD/include/bond/private/libruntime_embedded.h"
cppFile32="$PWD/source/private/libruntime32_embedded.h"
cppFile64="$PWD/source/private/libruntime64_embedded.h"
inputDir="lib/include"
buildDir32="build/libruntime32"
buildDir64="build/libruntime64"

make bondc
make embedfile

mkdir -p "$buildDir32"
mkdir -p "$buildDir64"

for file in "$inputDir"/*.bond
do
	cboName=$(basename "${file/%.bond/.cbo}")
	"$bondc" -p32 "$file" -o "$buildDir32/$cboName"
	"$bondc" -p64 "$file" -o "$buildDir64/$cboName"
done

pushd "$buildDir32"
"$embedfile" -h "$hFile" -c "$cppFile32" *.cbo

popd
pushd "$buildDir64"
"$embedfile" -c "$cppFile64" *.cbo
