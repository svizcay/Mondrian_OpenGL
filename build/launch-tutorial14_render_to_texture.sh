#!/bin/sh
bindir=$(pwd)
cd /home/orica/Downloads/OpenGL-tutorial_v0014_33/tutorial14_render_to_texture/
export LD_LIBRARY_PATH=:$LD_LIBRARY_PATH

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		 -batch -command=$bindir/gdbscript  /home/orica/Downloads/OpenGL-tutorial_v0014_33/build/tutorial14_render_to_texture 
	else
		"/home/orica/Downloads/OpenGL-tutorial_v0014_33/build/tutorial14_render_to_texture"  
	fi
else
	"/home/orica/Downloads/OpenGL-tutorial_v0014_33/build/tutorial14_render_to_texture"  
fi
