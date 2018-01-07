#!/bin/bash

#=====

stm32_dir=../../libstm32/share/lib

#=====

_path(){
	if [[ "$2" =~ ^(/|~) ]] ; then
		echo "$2"
	else
		echo "$1/$2"
	fi
}

src_dir=`dirname $0`
if [ "$src_dir" == '.' ] ; then
	build_dir=build
	mkdir -p $build_dir
	cd $build_dir
	src_dir='..'
fi
build_number="$1"

rm -rf *
mkdir -p lib/usb
ln -snf $src_dir/jtag .

ln -snf $stm32_dir $src_dir/lib
echo "SRCDIR = $src_dir" | cat - $src_dir/Config.mk $src_dir/Makefile.cfg > Makefile
echo "SRCDIR = `_path .. $src_dir/lib`" | cat - $src_dir/Config.mk $src_dir/lib/Makefile.cfg > lib/Makefile
echo "SRCDIR = `_path ../.. $src_dir/lib/usb`" | cat - $src_dir/Config.mk $src_dir/lib/usb/Makefile.cfg > lib/usb/Makefile

if [ -n "$build_number" ]
then
	echo "$build_number" > build_number.txt
fi
