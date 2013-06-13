#!bash

embedfile="$PWD/build/util/embedfile"
hFile32="$PWD/include/bond/private/libruntime32_embedded.h"
cppFile32="$PWD/source/private/libruntime32_embedded.h"
hFile64="$PWD/include/bond/private/libruntime64_embedded.h"
cppFile64="$PWD/source/private/libruntime64_embedded.h"
build32=lib/p32
build64=lib/p64

make bondc
make embedfile

mkdir -p "$build32"
mkdir -p "$build64"

for file in lib/include/*.bond
do
	cboName=$(basename "${file/%.bond/.cbo}")
	build/bin/bondc -p32 "$file" -o "$build32/$cboName"
	build/bin/bondc -p64 "$file" -o "$build64/$cboName"
done

pushd "$build32"
"$embedfile" -h "$hFile32" -c "$cppFile32" *.cbo

popd
pushd "$build64"
"$embedfile" -h "$hFile64" -c "$cppFile64" *.cbo
