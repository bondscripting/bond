#!bash

make bondc

for file in lib/include/*.bond
do
	name=$(basename "${file/%.bond/}")
	build/bin/bondc -c "source/lib${name}bindings.cpp" -h "include/bond/api/lib${name}.h" -i "bond/api/lib${name}.h" -b "Bond::${name^^}_BINDING_COLLECTION" "$file"
done
