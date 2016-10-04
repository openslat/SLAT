#! /bin/bash
DEST=export32
LIBBASE=/opt/mxe/usr/i686-w64-mingw32.shared
PYDIR=../python-win32

rm -rf $DEST
mkdir $DEST
cp example2.exe main.exe _pyslatcore.pyd pyslatcore.a \
   $DEST

old_libs=""
new_libs=$(ls $DEST)
until [ "$old_libs" == "$new_libs" ]; do
    old_libs="$new_libs"
    for lib in $DEST/*; do
	for f in $(objdump -p $lib | grep "DLL Name" | cut -d\  -f3); do
        echo $f;
        find $LIBBASE/bin $LIBBASE/lib $PYDIR . \
	     -name $DEST -prune \
	     -o -name $f \
	     -exec cp '{}' $DEST/$f \;
	done
    done
    new_libs=$(ls $DEST)
done

cp ../pyslat.py ../parser/*.py \
   ../parser/example2/example2.slat \
   ../parser/example2/example2.py \
   ../pyslatcore.py ../test.py \
   $DEST
