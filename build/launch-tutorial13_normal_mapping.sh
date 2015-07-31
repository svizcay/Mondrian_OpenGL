#!/bin/sh
bindir=$(pwd)
cd /home/orica/Downloads/OpenGL-tutorial_v0014_33/tutorial13_normal_mapping/
export LD_LIBRARY_PATH=:$LD_LIBRARY_PATH

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		 -batch -command=$bindir/gdbscript  /home/orica/Downloads/OpenGL-tutorial_v0014_33/build/tutorial13_normal_mapping 
	else
		"/home/orica/Downloads/OpenGL-tutorial_v0014_33/build/tutorial13_normal_mapping"  
	fi
else
	"/home/orica/Downloads/OpenGL-tutorial_v0014_33/build/tutorial13_normal_mapping"  
fi
