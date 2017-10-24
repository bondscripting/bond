#!bash

inputDir="lib/include"
buildDir="build/libdoc"

rm -rf "$buildDir"
mkdir -p "$buildDir"

for file in "$inputDir"/*.bond
do
	sed -e '/\/\/ begin ignore/,/\/\/ end ignore/d' "$file" > "$buildDir"/$(basename "$file")
done

doxygen bondapidox
