# strip_debug.sh <in> <out>

objcopy --strip-debug $1 $2 
strip $2
