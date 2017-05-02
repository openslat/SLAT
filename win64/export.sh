#! /bin/bash
DEST=openslat-64
LIBBASE=/opt/mxe/usr/x86_64-w64-mingw32.shared
PYDIR=~/swig-test/python-amd64

rm -rf $DEST
mkdir $DEST
mkdir $DEST/bin
cp bin/* lib/* scripts/* $DEST/bin


old_libs=""
new_libs=$(ls $DEST)
until [ "$old_libs" == "$new_libs" ]; do
    old_libs="$new_libs"
    for lib in $DEST/bin/*; do
	echo $lib
	for f in $(objdump -p $lib | grep "DLL Name" | cut -d\  -f3); do
        echo $f;
        find $LIBBASE/bin $LIBBASE/lib $PYDIR . \
	     -name $DEST -prune \
	     -o -name $f \
	     -exec cp '{}' $DEST/bin/$f \;
	done
    done
    new_libs=$(ls $DEST/bin)
done

mkdir $DEST/example2
mv $DEST/bin/example2.EXE $DEST/example2
cp -r ../parser/example2/input $DEST/example2
cp  ../parser/example2/example2.py $DEST/example2
cp  ../parser/example2/example2.slat $DEST/example2

cp runme.bat $DEST
cp readme.txt $DEST


