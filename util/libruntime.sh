#!/bin/bash

bondc="$PWD/build/bin/bondc"
embedfile="$PWD/build/util/embedfile"
inputDir="$PWD/lib/include"
buildDir32="$PWD/build/libruntime32"
buildDir64="$PWD/build/libruntime64"

make bondc
make embedfile

mkdir -p "$buildDir32"
mkdir -p "$buildDir64"

for file in "$inputDir"/*.bond
do
	cboName=$(basename "${file/%.bond/.cbo}")
	cboId="${cboName/./_}"
	cppFile32="$PWD/source/private/libruntime32_${cboId}_embedded.h"
	cppFile64="$PWD/source/private/libruntime64_${cboId}_embedded.h"

	pushd "$buildDir32"
	"$bondc" -p32 "$file" -o "$cboName"
	"$embedfile" -s -c "$cppFile32" "$cboName"
	popd

	pushd "$buildDir64"
	"$bondc" -p64 "$file" -o "$cboName"
	"$embedfile" -s -c "$cppFile64" "$cboName"
	popd
done
